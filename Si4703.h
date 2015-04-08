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
 *
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
#define SI4703_PIN_GPO2 2

//Constants for hardwired pins
//Chip interfaced via I2C, SEN tied high in hardware
//Define both HW and HWH constants to suit both semantics when typing
#define SI4703_PIN_SEN_HW 0xFF
#define SI4735_PIN_SEN_HWH 0xFF
//GPO/#INT not used, pin left floating
#define SI4735_PIN_GPO2_HW 0xFF

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
               byte pinGPO2 = SI4703_PIN_GPO2, byte pinSEN = SI4703_PIN_SEN);

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
        *   band        - The desired radio mode, one of the SI4703_BAND_*
        *                 constants.
        *   xosc        - A 32768Hz external oscillator is present.
        *   interrupt   - Interrupt mode is to be used (as opposed to polling)
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

    private:
        byte _pinReset, _pinGPO2, _pinSEN;
        static byte _i2caddr;
        bool _interrupt, _seeking;

        /*
        * Description:
        *   Waits for completion of various operations.
        * Parameters:
        *   which - interrupt flag to wait for, see SI4703_STATUS_*
        */
        void waitForInterrupt(byte which);

        /*
        * Description:
        *   Performs actions common to all tuning modes.
        */
        void completeTune(void);

        /*
        * Description:
        *   Services interrupts from the Si4703.
        */
        static void interruptServiceRoutine(void);
};

#endif
