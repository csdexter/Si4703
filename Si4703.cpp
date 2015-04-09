/* Arduino Si4703 (and family) Library
 * See the README file for author and licensing information. In case it's
 * missing from your distribution, use the one here as the authoritative
 * version: https://github.com/csdexter/Si4703/blob/master/README
 *
 * This library is for use with the SparkFun Si4703 Evaluation or Breakout
 * Boards.
 * See the example sketches to learn how to use the library in your code.
 *
 * This is the main code file for the library.
 * See the header file for better function documentation.
 */

#include "Si4703.h"
#include "Si4703-private.h"

#include <Wire.h>

Si4703::Si4703(byte pinReset, byte pinGPIO2, byte pinSEN) {
    _pinReset = pinReset;
    _pinGPIO2 = pinGPIO2;
    _pinSEN = pinSEN;
    _seeking = false;
}

void Si4703::begin(byte band, bool xosc, bool interrupt) {
    //Start by resetting the Si4703 and configuring the communication protocol
    pinMode(_pinReset, OUTPUT);
    pinMode(_pinSEN, OUTPUT);
    //I2C mode, SCLK is always connected to SCL
    pinMode(SCL, OUTPUT);

    //Put the Si4703 in reset
    digitalWrite(_pinReset, LOW);

    //Configure for I2C mode; GPIO3 is pulled low by internal pull-down
    digitalWrite(_pinSEN, HIGH);
    //SCLK must be high until we start talking to the chip
    digitalWrite(SCL, HIGH);

    //Use the longest of delays given in the datasheet
    delayMicroseconds(100);

    //Bring the Si4703 out of reset
    digitalWrite(_pinReset, HIGH);

    //Datasheet calls for 30ns delay; an Arduino running at 20MHz (4MHz
    //faster than the Uno, mind you) has a clock period of 50ns so no action
    //needed.

    //Configure GPIO2 for hardware interrupts if requested and possible.
    if(_pinGPIO2 != SI4703_PIN_GPIO2_HW && interrupt) pinMode(_pinGPIO2, INPUT);

    //Configure the I2C hardware
    Wire.begin();

    //Enable the crystal oscillator, if present
    if(xosc) {
        getRegisterBulk(true);
        _registers[SI4703_REG_TEST1] |= SI4703_FLG_XOSCEN;
        setRegisterBulk(true);
        //Wait for the oscillator to stabilize.
        delay(500);
    };

    //Cache the register file before powerup
    getRegisterBulk(true);

    //Ask the Si4703 to wake up
    _registers[SI4703_REG_POWERCFG] |= (SI4703_FLG_DMUTE | SI4703_FLG_ENABLE);
    setRegisterBulk();

    //Wait for it to finish booting
    delay(110);

    //Cache the register file after powerup
    getRegisterBulk(true);

    //Configure the Si4703 for operation
    _registers[SI4703_REG_SYSCONFIG1] |= SI4703_FLG_RDS;
    if(_pinGPIO2 != SI4703_PIN_GPIO2_HW)
        _registers[SI4703_REG_SYSCONFIG1] |= (
            SI4703_FLG_RDSIEN | SI4703_FLG_STCIEN | SI4703_GPIO2_INT);
    _registers[SI4703_REG_SYSCONFIG2] |= (
        band | SI4703_SPACE_100K | SI4703_VOLUME_MASK);
    setRegisterBulk();

    //The chip is alive and interrupts have been configured on its side, switch
    //ourselves to interrupt operation if so requested and if wiring was
    //properly done.
    _interrupt = interrupt && _pinGPIO2 != SI4703_PIN_GPIO2_HW;

    if (_interrupt) {
      attachInterrupt(_pinGPIO2, Si4703::interruptServiceRoutine, FALLING);
      interrupts();
    };
}

const byte Si4703_ChannelSpacings[3] PROGMEM = { 20, 10, 5 };

word Si4703::getFrequency(void) {
    getRegisterBulk();

    return (
        (_registers[SI4703_REG_SYSCONFIG2] & SI4703_BAND_MASK ==
            SI4703_BAND_WEST) ? 8750 : 7600) +
        _registers[SI4703_REG_READCHAN] & SI4703_READCHAN_MASK *
        pgm_read_byte(&Si4703_ChannelSpacings[
            _registers[SI4703_REG_SYSCONFIG2] & SI4703_SPACE_MASK >> 4]);
}

void Si4703::seekUp(bool wrap) {
    _registers[SI4703_REG_POWERCFG] |= ((wrap ? 0x00 : SI4703_FLG_SKMODE) |
                                        SI4703_FLG_SEEKUP | SI4703_FLG_SEEK);
    setRegisterBulk();

    completeTune();
}

void Si4703::seekDown(bool wrap) {
    _registers[SI4703_REG_POWERCFG] |= ((wrap ? 0x00 : SI4703_FLG_SKMODE) |
                                        SI4703_FLG_SEEK);
    setRegisterBulk();

    completeTune();
}

byte Si4703::getRSSI(void) {
    getRegisterBulk();

    return _registers[SI4703_REG_STATUSRSSI] & SI4703_RSSI_MASK;
}

bool Si4703::volumeUp(void) {
    getRegisterBulk();

    const byte volume = _registers[SI4703_REG_SYSCONFIG2] & SI4703_VOLUME_MASK;

    if(volume == SI4703_VOLUME_MASK)
        return false;
    else {
        _registers[SI4703_REG_SYSCONFIG2] = _registers[SI4703_REG_SYSCONFIG2] &
                                            ~SI4703_VOLUME_MASK | (volume + 1);
        setRegisterBulk();

        return true;
    };
}

bool Si4703::volumeDown(bool alsomute) {
    getRegisterBulk();

    const byte volume = _registers[SI4703_REG_SYSCONFIG2] & SI4703_VOLUME_MASK;

    if(volume) {
        _registers[SI4703_REG_SYSCONFIG2] = _registers[SI4703_REG_SYSCONFIG2] &
                                            ~SI4703_VOLUME_MASK | (volume - 1);
        setRegisterBulk();
        if(!(volume - 1) && alsomute)
            //If we are to trust the datasheet, this is superfluous as a volume
            //of zero triggers mute on its own.
            mute();
        return true;
    } else
        return false;
}

void Si4703::unMute(bool minvol) {
    if(minvol)
        _registers[SI4703_REG_SYSCONFIG2] = _registers[SI4703_REG_SYSCONFIG2] &
                                            SI4703_VOLUME_MASK | 0x1;
    _registers[SI4703_REG_POWERCFG] |= SI4703_FLG_DMUTE;

    setRegisterBulk();
}

void Si4703::mute(void) {
    _registers[SI4703_REG_POWERCFG] &= ~SI4703_FLG_DMUTE;

    setRegisterBulk();
};

void Si4703::end(void) {
    mute();
    _registers[SI4703_REG_POWERCFG] |= SI4703_FLG_DISABLE;
    _registers[SI4703_REG_SYSCONFIG1] &= ~SI4703_FLG_RDS;

    setRegisterBulk();
}

void Si4703::sendCommand(byte command, byte arg0, byte arg1, byte arg2,
                         byte arg3, byte arg4, byte arg5, byte arg6) {
    const bool previousRDS = _registers[SI4703_REG_SYSCONFIG1] & SI4703_FLG_RDS;

    //Enable command processor
    _registers[SI4703_REG_SYSCONFIG1] &= ~SI4703_FLG_RDS;
    _registers[SI4703_REG_RDSD] = word(0x00, SI4703_CMD_VERIFY_COMMAND);
    setRegisterBulk(false, true);
    //Wait for activation
    while(_registers[SI4703_REG_RDSD])
        getRegisterBulk();

    //Send the command and its arguments
    _registers[SI4703_REG_RDSA] = word(arg0, arg1);
    _registers[SI4703_REG_RDSB] = word(arg2, arg3);
    _registers[SI4703_REG_RDSC] = word(arg4, arg5);
    _registers[SI4703_REG_RDSD] = word(arg6, command);
    setRegisterBulk(false, true);

    //Wait for processing
    while(lowByte(_registers[SI4703_REG_RDSD]))
        getRegisterBulk();
    //Copy the (now valid) response bytes over as re-enabling RDS below may
    //immediately trigger an interrupt which will clobber our data.
    _response[0] = _registers[SI4703_REG_RDSA];
    _response[1] = _registers[SI4703_REG_RDSB];
    _response[2] = _registers[SI4703_REG_RDSC];
    _response[3] = _registers[SI4703_REG_RDSD];

    //Restore previous RDS state
    if(previousRDS) {
        _registers[SI4703_REG_SYSCONFIG1] |= SI4703_FLG_RDS;
        setRegisterBulk();
    };
};

void Si4703::setProperty(word property, word value) {
    sendCommand(SI4703_CMD_SET_PROPERTY, highByte(value), lowByte(value), 0, 0,
                highByte(property), lowByte(property));
}

word Si4703::getProperty(word property) {
    sendCommand(SI4703_CMD_GET_PROPERTY, 0, 0, 0, 0, highByte(property),
                lowByte(property));

    return _response[0];
}

void Si4703::getRegisterBulk(bool all) {
    const byte count = all ? SI4703_LAST_REGISTER : 6;

    Wire.requestFrom(SI4703_I2C_ADDR, count * 2);

    for(byte i = 0; i < count; i++) {
        _registers[
            (SI4703_FIRST_REGISTER_READ + i) & SI4703_LAST_REGISTER] =
                (word)Wire.read() << 8;
        _registers[
            (SI4703_FIRST_REGISTER_READ + i) & SI4703_LAST_REGISTER] |=
                Wire.read();
    };
};

void Si4703::setRegisterBulk(bool test, bool cmd) {
    Wire.beginTransmission(SI4703_I2C_ADDR);

    for(byte i = 0; i < (cmd ? 14 : (test ? 6 : 5)); i++) {
        Wire.write(highByte(_registers[SI4703_FIRST_REGISTER_WRITE + i]));
        Wire.write(lowByte(_registers[SI4703_FIRST_REGISTER_WRITE + i]));
    };

    Wire.endTransmission();
};

void Si4703::waitForInterrupt(word which) {
    while(!(_registers[SI4703_REG_STATUSRSSI] & which))
        if(!_interrupt) {
            if(which == SI4703_STATUS_STC)
                //Give the chip a rest while it's seeking/tuning, according to
                //datasheet recommendations.
                delay(60);
            getRegisterBulk();
        };
}

void Si4703::completeTune(void) {
    waitForInterrupt(SI4703_STATUS_STC);

    //Reset STC and SF/BL flags
    _registers[SI4703_REG_POWERCFG] &= ~SI4703_FLG_SEEK;
}

void Si4703::interruptServiceRoutine(void) {
    getRegisterBulk();
}

volatile word Si4703::_registers[] = {0x0000};
