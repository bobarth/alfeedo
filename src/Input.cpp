/*
This file is part of alfeedo.

alfeedo is free software: you can redistribute it and/or modify it under the 
terms of the GNU General Public License as published by the Free Software 
Foundation, either version 3 of the License, or (at your option) any later version.

alfeedo is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with alfeedo. 
If not, see <https://www.gnu.org/licenses/>. 

Copyright (C) 2025-2026 Michael Zanetti <michael_zanetti@gmx.net>
*/

#include "Input.h"
#include "Engine.h"
#include "Logging.h"

LoggingCategory lcInput("Input");

#define DEBOUNCE_DELAY 80 // milliseconds (touch pads: slightly longer than mechanical switches)
#define BOOT_LOCKOUT 2000 // ignore presses for this long after boot (milliseconds)

void Input::setup(uint8_t pin, InputHandler handler)
{
    m_handler = handler;
    m_pin = pin;

    // Internal pull-up so an unconnected or floating pin reads HIGH (not pressed)
    pinMode(pin, INPUT_PULLUP);
    delay(5);

    // Start from the real pin level: a pin stuck LOW at boot must be released
    // before it can trigger anything.
    int initial = digitalRead(pin);
    m_buttonState = initial;
    m_lastButtonState = initial;
    m_lastDebounceTime = 0;

    logInfo(lcInput, "GPIO" + String(pin) + " set up.");
}

void Input::loop()
{
    unsigned long now = millis();

    int reading = digitalRead(m_pin);

    if (reading != m_lastButtonState) {
        m_lastDebounceTime = now;
    }

    if ((now - m_lastDebounceTime) > DEBOUNCE_DELAY) {
        if (reading != m_buttonState) {
            m_buttonState = reading;
            if (m_buttonState == LOW && now > BOOT_LOCKOUT) {
                logDebug(lcInput, "Button (GPIO" + String(m_pin) + " pressed.");
                m_handler();
            }
        }
    }
    m_lastButtonState = reading;
}

