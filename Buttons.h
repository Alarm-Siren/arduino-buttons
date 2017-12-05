/*
 *  Arduino Buttons Library
 *  An interrupt-driven, fully-debounced class to manage input from physical buttons on the Arduino platform.
 *
 *  Copyright (C) 2017 Nicholas Parks Young
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
 *  USA
 */

#ifndef BUTTONS_H
#define BUTTONS_H

#include <Arduino.h>

/**
 * This static-only class implements a system for getting user input from buttons.
 * It internally applies debounce periods and tracks whether a button press or release
 * has been "processed" by use of a Change Flag for each button.
 *
 * This is all interrupt driven, so there is no penalty to running code except when the
 * user actually presses a button. This also means, implicitly, you must ensure that
 * any pins used for button inputs are capable of having interrupts attached to them.
 * On the Arduino Due (for example) all digital pins can be used in this way, but on
 * the Arduino Uno, only pins 2 and 3 can have interrupts attached.
 *
 * Website: https://github.com/Alarm-Siren/arduino-buttons
 *
 *  DEFINITION FILE
 *
 * @author      Nicholas Parks Young
 * @version     1.0
 */
class Buttons final 
{
  public:

    /**
     * Initialize the buttons as attached to the specified pins and attach appropriate interrupts.
     * The index of each button in the buttonPins parameter array is preserved for the buttonId parameter
     * on accessor methods such as clicked, down etc. Hence, if you want to read the status of
     * the button attached to the pin specified in buttonPins[3], you could call clicked(3, true).
     *
     * @param buttonPins        pointer to an array of bytes, each being the number of a
     *                          pin with a button attached that is to be managed by this object.
     * @param numberOfButtons   Number of buttons and size of the buttonPins array
     * @return                  true on success, false on failure.
     */
    static boolean begin(const byte* const buttonPins, byte numberOfButtons);

    /**
     * TO DO
     */
    static boolean begin(std::initializer_list<byte> buttonPins);
    
    /**
     * Detach interrupts from the pins controlled by this object and free up all associated memory.
     * If the object has not been started with begin(), or begin() failed, calling this will do nothing.
     */
    static void stop();

    /**
     * Returns a boolean value indicating if the user has "clicked" the button,
     * defined as the button being down and the Change Flag set.
     *
     * @param buttonId          Index of the button whose status is to be checked.
     * @param clearChangeFlag   If true, the Change Flag for this button will be cleared at the same time.
     * @return                  true if the button has been clicked since the Change Flag
     *                          was last cleared, false otherwise.
     */
    static boolean clicked(byte buttonId, boolean clearChangeFlag);
    
    /**
     * Returns a boolean value indicating if the user has "released" the button,
     * defined as the button being up and the Change Flag set.
     *
     * @param buttonId          Index of the button whose status is to be checked.
     * @param clearChangeFlag   If true, the Change Flag for this button will be cleared at the same time.
     * @return                  true if the button has been clicked since the Change Flag
     *                          was last cleared, false otherwise.
     */
    static boolean released(byte buttonId, boolean clearChangeFlag);
    
    /**
     * Returns a boolean value indicating if the button is currently "down"/"pressed".
     * This return value is independent of the state of the Change Flag, however you can
     * clear the Change Flag if desired.
     * It is the opposite of the up() method.
     *
     * @param buttonId          Index of the button whose status is to be checked.
     * @param clearChangeFlag   If true, the Change Flag for this button will be cleared at the same time.
     * @return                  true if the button is down.
     */
    static boolean down(byte buttonId, boolean clearChangeFlag);

    /**
     * Returns a boolean value indicating if the button is currently "up"/"not pressed".
     * This return value is independent of the state of the Change Flag, however you can
     * clear the Change Flag if desired.
     * It is the opposite of the down() method.
     *
     * @param buttonId          Index of the button whose status is to be checked.
     * @param clearChangeFlag   If true, the Change Flag for this button will be cleared at the same time.
     * @return                  true if the button is up.
     */
    static boolean up(byte buttonId, boolean clearChangeFlag);

    /**
     * Returns a boolean value indicating if the button's state has changed since the 
     * Change Flag was last cleared.
     * This return value is independent of whether the button itself is up or down, it
     * need only have changed.
     *
     * @param buttonId          Index of the button whose status is to be checked.
     * @param clearChangeFlag   If true, the Change Flag for this button will be cleared at the same time.
     * @return                  true if the button's state has changed.
     */
    static boolean changed(byte buttonId, boolean clearChangeFlag);

    /**
     * This method clears all Change Flags for all buttons.
     * Useful to call when entering or leaving a user-interaction context so that spurious
     * button presses during the "non-interactive" phase do not trigger an unexpected action.
     */
    static void clearAllChangeFlags();

    /**
     * Returns the number of buttons currently controlled by this class.
     *
     * @return    The number of buttons controlled by this class
     */
    static byte numberOfButtons();

    //This class has only static members, therefore constructors etc are pointless.
    Buttons() = delete;
    ~Buttons() = delete;
    Buttons& operator=(const Buttons&) = delete;
    Buttons(const Buttons&) = delete;

  private:

    /**
     * Debounce period in milliseconds.
     */
    static unsigned long DEBOUNCE_DELAY = 50;

    /**
     * This structure encompasses information relating to an individual button.
     */
    struct Button
    {
      /**
       * Stores the most recently measured state of the button.
       * true = pushed, false = not pushed.
       */
      boolean currentState;

      /**
       * This flag indicates is set to true when currentState changes,
       * and (optionally) set false when that state is read.
       */
      boolean changeFlag;

      /**
       * This records the last time that an Interrupt was triggered from this pin.
       * Used as part of the debounce routine.
       */
      unsigned long lastChangeTime;

      /**
       * Constructor for objects of Button.
       */
      Button() :
        currentState(false),
        changeFlag(false),
        lastChangeTime(0)
      { }
    };

    /**
     * This function is called whenever a button interrupt is fired.
     * It reads all the button states and updates their _buttonStatus objects
     * accordingly.
     */
    static void button_ISR();

    /**
     * Stores the number of buttons controlled by this class,
     * which is also the size of the _buttonPins and _buttonStatus arrays.
     */
    static byte _numberOfButtons;

    /**
     * This array stores pin numbers for each button controlled by this class.
     */
    static byte* _buttonPins;

    /**
     * This array stores Button objects for each button controlled by this class,
     * each containing relevant information for the servicing of the ISR.
     * Its volatile because its members may be modified by an ISR, so we need to
     * prevent register caching of member values.
     */
    static volatile Button* _buttonStatus;

    /**
     * Set to true if this class has been initialised, false otherwise.
     */
    static boolean _begun;
};

#endif
