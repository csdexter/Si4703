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

Si4703::Si4703(byte pinReset, byte pinGPIO2, byte pinSEN){
    _pinReset = pinReset;
    _pinGPIO2 = pinGPIO2;
    _pinSEN = pinSEN;
}

void Si4703::begin(byte band, bool xosc, bool interrupt){
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

word Si4703::getFrequency(void){
    return word(0x0000);
}

void Si4703::seekUp(bool wrap){
    completeTune();
}

void Si4703::seekDown(bool wrap){
    completeTune();
}

byte Si4703::getRSSI(void){
    return 0x00;
}

bool Si4703::volumeUp(void){
    return false;
}

bool Si4703::volumeDown(bool alsomute){
    return false;
}

void Si4703::unMute(bool minvol){
}

void Si4703::mute(void){
};

void Si4703::end(void){
    digitalWrite(_pinReset, LOW);
}

void Si4703::setProperty(word property, word value){
}

word Si4703::getProperty(word property){
    return word(0x0000);
}

void Si4703::waitForInterrupt(byte which){
}

void Si4703::completeTune(void) {
}

void Si4703::interruptServiceRoutine(void) {
};

volatile word Si4703::_registers[] = {0x0000};