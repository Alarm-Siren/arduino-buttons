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

/**
 * This static-only class implements a system for getting user input from buttons.
 * It internally applies debounce periods and tracks whether a button press or release
 * has been "processed".
 *
 * This is all interrupt driven, so there is no penalty to running code except when the
 * user actually presses a button. This also means, implicitly, you must ensure that
 * any pins used for button inputs are capable of having interrupts attached to them.
 * On the Arduino Due (for example) all digital pins can be used in this way, but on
 * the Arduino Uno, only pins 2 and 3 can have interrupts attached.
 *
 * Website: https://github.com/Alarm-Siren/arduino-buttons
 *
 *  IMPLEMENTATION FILE
 *
 * @author      Nicholas Parks Young
 * @version     1.0
 */

#include "buttons.hpp"

size_t Buttons::_numberOfButtons = 0;
byte* Buttons::_buttonPins = nullptr;
volatile Buttons::Button* Buttons::_buttonStatus = nullptr;
boolean Buttons::_begun = false;

boolean Buttons::begin(const byte* const buttonPins, size_t numberOfButtons)
{
  // Abort if the buttonPins array is null
  if (nullptr == buttonPins) return false;
  
  // If Buttons has already been started, kill it before restarting it.
  if (_begun) stop();

  // Setup internal storage buffers, etc.
  _numberOfButtons = numberOfButtons;
  _buttonPins = new byte[numberOfButtons];
  _buttonStatus = new Button[numberOfButtons];

  //Make sure that the memory was successfully allocated.
  if (!_buttonPins || !_buttonStatus) {
    return false;
  }

  // Set up the input pins themselves.
  for (size_t i = 0; i < numberOfButtons; i++) {
    _buttonPins[i] = buttonPins[i];
    pinMode(buttonPins[i], INPUT_PULLUP);
  }
  
  // Need to wait some time before setting up the ISRs, otherwise you can get spurious
  // changes as the pullup hasn't quite done its magic yet.
  delay(10);

  //Set up the interrupts on the pins.
  for (size_t i = 0; i < numberOfButtons; i++) {
    attachInterrupt(digitalPinToInterrupt(buttonPins[i]), &Buttons::button_ISR, CHANGE);
  }

  // All done.
  _begun = true;
  return true;
}

void Buttons::stop()
{
  // If the object is already stopped, we don't need to do anything.
  if (!_begun)
    return;
  
  //Disable the interrupts
  for (size_t i = 0; i < _numberOfButtons; i++) {
    detachInterrupt(digitalPinToInterrupt(_buttonPins[i]));
  }
  
  //Destroy dynamic memory.
  delete[] _buttonPins;
  delete[] _buttonStatus;
  
  //Object has been stopped.
  _begun = false;
}

void Buttons::button_ISR()
{
  for (size_t i = 0; i < _numberOfButtons; i++) {
    const boolean readState = !digitalRead(_buttonPins[i]);
    if (readState != _buttonStatus[i].currentState) {
      if (millis() > _buttonStatus[i].lastChangeTime + DEBOUNCE_DELAY) {
        _buttonStatus[i].currentState = readState;
        _buttonStatus[i].changeFlag = true;
      }
      _buttonStatus[i].lastChangeTime = millis();
    }
  }
}

boolean Buttons::clicked(size_t buttonId, boolean clearChangeFlag)
{
  return changed(buttonId, clearChangeFlag) && down(buttonId, false);
}

boolean Buttons::down(size_t buttonId, boolean clearChangeFlag)
{
  if (!_begun)
    return false;
  
  if (clearChangeFlag)
    _buttonStatus[buttonId].changeFlag = false;
  
  return  _buttonStatus[buttonId].currentState;
}

boolean Buttons::up(size_t buttonId, boolean clearChangeFlag)
{
  return !down(buttonId, clearChangeFlag);
}

boolean Buttons::changed(size_t buttonId, boolean clearChangeFlag)
{
  if (!_begun)
    return false;
  
  //Save current changeFlag before optionally clearing it.
  const boolean answer = _buttonStatus[buttonId].changeFlag;
  if (clearChangeFlag)
    _buttonStatus[buttonId].changeFlag = false;
  
  return answer;
}

void Buttons::clearAllChangeFlags()
{
  if (!_begun)
    return;
  
  for (size_t i = 0; i < _numberOfButtons; i++) {
    _buttonStatus[i].changeFlag = false;
  }
}

size_t Buttons::numberOfButtons()
{
  if (_begun) {
    return _numberOfButtons;
  } else {
    return 0;
  }
}

