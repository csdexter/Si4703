/* Arduino Si4703 (and family) Library
 * See the README file for author and licensing information. In case it's
 * missing from your distribution, use the one here as the authoritative
 * version: https://github.com/csdexter/Si4703/blob/master/README
 *
 * This library is for use with the SparkFun Si4703 Evaluation or Breakout
 * Boards.
 * See the example sketches to learn how to use the library in your code.
 *
 * This is the main include file for the library.
 */

#ifndef _SI4703_H_INCLUDED
#define _SI4703_H_INCLUDED

#if defined(ARDUINO) && ARDUINO >= 100
# include <Arduino.h>
#else
# include <WProgram.h>
#endif

//Assign the pin numbers (evaluation or breakout board version)
//SDIO and SCLK always connected to SDA and SCL
#define SI4703_PIN_SEN SS
#define SI4703_PIN_RESET 9
#define SI4703_PIN_GPIO2 2

//Constants for hardwired pins
//Chip interfaced via I2C, SEN tied high in hardware
//Define both HW and HWH constants to suit both semantics when typing
#define SI4703_PIN_SEN_HW 0xFF
#define SI4703_PIN_SEN_HWH 0xFF
//GPIO2/#INT not used, pin left floating
#define SI4703_PIN_GPIO2_HW 0xFF

//Register file origins for sequential mode
#define SI4703_FIRST_REGISTER_WRITE 0x02
#define SI4703_FIRST_REGISTER_READ 0x0A
#define SI4703_LAST_REGISTER 0x0F

//Si4703 register addresses
#define SI4703_REG_DEVICEID 0x00
#define SI4703_REG_CHIPID 0x01
#define SI4703_REG_POWERCFG 0x02
#define SI4703_REG_CHANNEL 0x03
#define SI4703_REG_SYSCONFIG1 0x04
#define SI4703_REG_SYSCONFIG2 0x05
#define SI4703_REG_SYSCONFIG3 0x06
#define SI4703_REG_TEST1 0x07
#define SI4703_REG_TEST2 0x08
#define SI4703_REG_BOOTCONFIG 0x09
#define SI4703_REG_STATUSRSSI 0x0A
#define SI4703_REG_READCHAN 0x0B
#define SI4703_REG_RDSA 0x0C
#define SI4703_REG_RDSB 0x0D
#define SI4703_REG_RDSC 0x0E
#define SI4703_REG_RDSD 0x0F

//Status bits (from the chip)
#define SI4703_STATUS_RDSR 0x8000
#define SI4703_STATUS_STC 0x4000
#define SI4703_STATUS_SFBL 0x2000
#define SI4703_STATUS_AFCRL 0x1000
#define SI4703_STATUS_RDSS 0x0800
#define SI4703_STATUS_ST 0x0100

//Flag bits (to the chip)
#define SI4703_FLG_DSMUTE 0x8000
#define SI4703_FLG_DMUTE 0x4000
#define SI4703_FLG_MONO 0x2000
#define SI4703_FLG_RDSM 0x0800
#define SI4703_FLG_SKMODE 0x0400
#define SI4703_FLG_SEEKUP 0x0200
#define SI4703_FLG_SEEK 0x0100
#define SI4703_FLG_DISABLE word(0x0040)
#define SI4703_FLG_ENABLE word(0x0001)
#define SI4703_FLG_TUNE 0x8000
#define SI4703_FLG_RDSIEN 0x8000
#define SI4703_FLG_STCIEN 0x4000
#define SI4703_FLG_RDS 0x1000
#define SI4703_FLG_DE 0x0800
#define SI4703_FLG_AGCD 0x0400
#define SI4703_FLG_VOLEXT 0x0100
#define SI4703_FLG_XOSCEN 0x8000
#define SI4703_FLG_AHIZEN 0x4000

//Masks and constants for configuration parameters
#define SI4703_PN_MASK 0xF000
#define SI4703_PN_SI4700_1 (0x0 << 12)
#define SI4703_PN_SI4702_3 (0x1 << 12)
#define SI4703_MFGID_MASK 0x0FFF
#define SI4703_MFGID_SILABS 0x0242
#define SI4703_REV_MASK 0xFC00
#define SI4703_REV_C (0x04 << 10)
#define SI4703_DEV_MASK 0x03C0
#define SI4703_DEV_SI4702_OFF (0x0 << 6)
#define SI4703_DEV_SI4702_ON (0x1 << 6)
#define SI4703_DEV_SI4703_OFF (0x8 << 6)
#define SI4703_DEV_SI4703_ON (0x9 << 6)
#define SI4703_FIRMWARE_MASK word(0x003F)
#define SI4703_FIRMWARE_OFF 0x00
#define SI4703_FIRMWARE_ON 0x13
#define SI4703_CHAN_MASK 0x03FF
#define SI4703_BLNDADJ_MASK word(0x00C0)
#define SI4703_BLNDADJ_31_49DB (0x0 << 6)
#define SI4703_BLNDADJ_37_55DB (0x1 << 6)
#define SI4703_BLNDADJ_19_37DB (0x2 << 6)
#define SI4703_BLNDADJ_25_43DB (0x3 << 6)
#define SI4703_GPIO3_MASK word(0x0030)
#define SI4703_GPIO3_HIZ (0x0 << 4)
#define SI4703_GPIO3_ST (0x1 << 4)
#define SI4703_GPIO3_L (0x2 << 4)
#define SI4703_GPIO3_H (0x3 << 4)
#define SI4703_GPIO2_MASK word(0x000C)
#define SI4703_GPIO2_HIZ (0x0 << 2)
#define SI4703_GPIO2_INT (0x1 << 2)
#define SI4703_GPIO2_L (0x2 << 2)
#define SI4703_GPIO2_H (0x3 << 2)
#define SI4703_GPIO1_MASK word(0x0003)
#define SI4703_GPIO1_HIZ 0x0
#define SI4703_GPIO1_L 0x2
#define SI4703_GPIO1_H 0x3
#define SI4703_SEEKTH_MASK 0xFF00
#define SI4703_SEEKTH_SHIFT 8
#define SI4703_BAND_MASK word(0x00C0)
#define SI4703_BAND_WEST (0x0 << 6)
#define SI4703_BAND_WORLD (0x1 << 6)
#define SI4703_BAND_EAST (0x2 << 6)
#define SI4703_SPACE_MASK word(0x0030)
#define SI4703_SPACE_200K (0x0 << 4)
#define SI4703_SPACE_100K (0x1 << 4)
#define SI4703_SPACE_50K (0x2 << 4)
#define SI4703_VOLUME_MASK word(0x000F)
#define SI4703_SMUTER_MASK 0xC000
#define SI4703_SMUTER_FASTEST (0x0 << 14)
#define SI4703_SMUTER_FAST (0x1 << 14)
#define SI4703_SMUTER_SLOW (0x2 << 14)
#define SI4703_SMUTER_SLOWEST (0x3 << 14)
#define SI4703_SMUTEA_MASK 0x3000
#define SI4703_SMUTEA_16DB (0x0 << 12)
#define SI4703_SMUTEA_14DB (0x1 << 12)
#define SI4703_SMUTEA_12DB (0x2 << 12)
#define SI4703_SMUTEA_10DB (0x3 << 12)
#define SI4703_SKSNR_MASK word(0x00F0)
#define SI4703_SKSNR_SHIFT 4
#define SI4703_SKCNT_MASK word(0x000F)
#define SI4703_BLERA_MASK 0x0600
#define SI4703_BLERA_0 (0x0 << 9)
#define SI4703_BLERA_12 (0x1 << 9)
#define SI4703_BLERA_35 (0x2 << 9)
#define SI4703_BLERA_U (0x3 << 9)
#define SI4703_RSSI_MASK word(0x00FF)
#define SI4703_BLERB_MASK 0xC000
#define SI4703_BLERB_0 (0x0 << 14)
#define SI4703_BLERB_12 (0x1 << 14)
#define SI4703_BLERB_35 (0x2 << 14)
#define SI4703_BLERB_U (0x3 << 14)
#define SI4703_BLERC_MASK 0x3000
#define SI4703_BLERC_0 (0x0 << 12)
#define SI4703_BLERC_12 (0x1 << 12)
#define SI4703_BLERC_35 (0x2 << 12)
#define SI4703_BLERC_U (0x3 << 12)
#define SI4703_BLERD_MASK 0x0C00
#define SI4703_BLERD_0 (0x0 << 10)
#define SI4703_BLERD_12 (0x1 << 10)
#define SI4703_BLERD_35 (0x2 << 10)
#define SI4703_BLERD_U (0x3 << 10)
#define SI4703_READCHAN_MASK 0x03FF

//Commands
#define SI4703_CMD_SET_PROPERTY 0x07
#define SI4703_CMD_GET_PROPERTY 0x08
#define SI4703_CMD_VERIFY_COMMAND 0xFF

//Properties
#define SI4703_PROP_FM_DETECTOR_SNR 0x0200
#define SI4703_PROP_BLEND_MONO_RSSI 0x0300
#define SI4703_PROP_BLEND_STEREO_RSSI 0x0301
#define SI4703_PROP_CALCODE 0x0700
#define SI4703_PROP_SNRDB 0x0C00

extern const byte Si4703_ChannelSpacings[];

class Si4703
{
    public:
        /*
        * Description:
        *   This the constructor. It assumes SparkFun Si4703 Evaluation or
        *   Breakout Board + level shifter (or 3.3V-native Arduino) semantics
        *   if called without parameters.
        *   If you're not using the SparkFun Evaluation or Breakout Boards or
        *   have wired the Si4703 differently, then explicitly supply the
        *   constructor with the actual pin numbers.
        *   Use the hardwired pins constants above to tell the constructor you
        *   haven't used (and hardwired) some of the pins.
        * Parameters:
        *   pin*      - pin numbers for connections to the Si4703, with
        *               defaults for the SparkFun Evaluation or Breakout 
        *               Boards already provided.
        */
        Si4703(byte pinReset = SI4703_PIN_RESET,
               byte pinGPIO2 = SI4703_PIN_GPIO2, byte pinSEN = SI4703_PIN_SEN);

        /*
        * Description:
        *   This is the destructor, it delegates to end().
        */
        ~Si4703() { end(); };

        /*
        * Description:
        *   Initializes the Si4703 and powers up the radio with the desired
        *   band limits.
        *   This function must be called before any other radio command.
        * Parameters:
        *   band        - the desired FM band, one of the SI4703_BAND_*
        *                 constants.
        *   xosc        - a 32768Hz external oscillator is present.
        *   interrupt   - interrupt mode is to be used (as opposed to polling)
        *                 when waiting for the chip to perform an operation.
        */
        void begin(byte band, bool xosc = true, bool interrupt = false);

        /*
        * Description:
        *   Gets the frequency the chip is currently tuned to.
        * Returns:
        *   frequency in 10kHz units.
        */
        word getFrequency(void);

        /*
        * Description:
        *   Commands the radio to seek up to the next valid channel.
        * Parameters:
        *   wrap - set to true to allow the seek to wrap around the current
        *          band.
        */
        void seekUp(bool wrap = true);

        /*
        * Description:
        *   Commands the radio to seek down to the next valid channel.
        * Parameters:
        *   wrap - set to true to allow the seek to wrap around the current
        *          band.
        */
        void seekDown(bool wrap = true);

        /*
        * Description:
        *   Retrieves the Received Signal Strength Indication measurement for
        *   the currently tuned station.
        */
        byte getRSSI(void);

        /*
        * Description:
        *   Increase the volume by 1. If the maximum volume has been
        *   reached, no further increase will take place and returns false;
        *   otherwise true.
        */
        bool volumeUp(void);

        /*
        * Description:
        *   Decrease the volume by 1. If the minimum volume has been
        *   reached, no further decrease will take place and returns false;
        *   otherwise true.
        * Parameters:
        *   alsomute - mute the output when reaching minimum volume, in
        *               addition to returning false
        */
        bool volumeDown(bool alsomute = false);

        /*
        * Description:
        *   Mutes the audio output.
        */
        void mute(void);

        /*
        * Description:
        *   Unmutes the audio output.
        * Parameters:
        *   minvol - set the volume to minimum value before unmuting if true,
        *            otherwise leave it untouched causing the chip to blast
        *            audio out at whatever the previous volume level was.
        */
        void unMute(bool minvol = false);

        /*
        * Description:
        *   Mutes and disables the chip.
        */
        void end(void);

        /*
        * Description:
        *   Sets a property value, see the SI4703_PROP_* constants and the
        *   Si4703 Datasheet for more information.
        */
        void setProperty(word property, word value);

        /*
        * Description:
        *   Gets a property value, see the SI4703_PROP_* constants and the
        *   Si4703 Datasheet for more information.
        * Returns:
        *   The current value of property.
        */
        word getProperty(word property);

        /*
        * Description:
        *   Accessor for the status register.
        */
        word getStatus(void) { return _registers[SI4703_REG_STATUSRSSI]; };

        /*
        * Description:
        *   If the chip has received any valid RDS group, fetch it off the chip
        *   and fill word block[4] with it, returning true; otherwise return
        *   false without side-effects.
        *   As RDS has a [mandated by standard] constant transmission rate of
        *   11.4 groups per second, you should actively call this function (e.g.
        *   from loop()) so that you read most if not all of the error-corrected
        *   RDS groups received. For example:
        *   loop() {
        *     if(Si4703::readRDSGroup(data))
        *       RDSDecoder::decodeRDSGroup(data);
        *   }
        */
        bool readRDSGroup(word* block);

    private:
        byte _pinReset, _pinGPIO2, _pinSEN;
        bool _interrupt, _seeking;
        static volatile word _registers[SI4703_LAST_REGISTER];
        word _response[4];
        static volatile word _rdsBlocks[4];
        static volatile bool _haveRds;

        /*
        * Description:
        *   Used to send a command and its arguments to the radio chip.
        * Parameters:
        *   command - the command byte, see datasheet and use one of the
        *             SI4703_CMD_* constants
        *   arg1-7  - command arguments, see the Si4703 Programmers Guide.
        */
        void sendCommand(byte command, byte arg0 = 0, byte arg1 = 0,
                         byte arg2 = 0, byte arg3 = 0, byte arg4 = 0,
                         byte arg5 = 0, byte arg6 = 0);

        /*
        * Description:
        *   Update the register file in bulk as the Si4703 doesn't support
        *   random access to its registers via I2C.
        * Parameters:
        *   all  - read the entire register file, as opposed to just the
        *          readable registers (0xA->0xF)
        *   test - include register TEST1 (0x7) in the write operation,
        *          otherwise stop at the writable registers (0x2->0x6)
        *   cmd  - a command is to be sent via the RDS registers, write the
        *          entire register file up to 0xF
        */
        static void getRegisterBulk(bool all = false);
        void setRegisterBulk(bool test = false, bool cmd = false);

        /*
        * Description:
        *   Waits for completion of various operations.
        * Parameters:
        *   which - interrupt flag to wait for, see SI4703_STATUS_*
        */
        void waitForInterrupt(word which);

        /*
        * Description:
        *   Performs actions common to all tuning modes.
        */
        void completeTune(void);

        /*
        * Description:
        *
        * Services interrupts from the Si4703.
        */
        static void interruptServiceRoutine(void);
};

#endif
