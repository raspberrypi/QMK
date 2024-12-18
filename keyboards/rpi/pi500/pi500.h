/* Copyright 2023 Raspberry Pi
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include "quantum.h"

#define PWR_BTN GP19
#define CAPS_LED GP25
#define LED_PIN GP17

// Set to 1 to enable power button debounce stuff
#define PWR_BUTTON_DELAY_ENABLED 0

#if PWR_BUTTON_DELAY_ENABLED
// The time in ms you need to hold down the power button for it to "press down"
#define PWR_DOWN_WAIT_TIME_MS 100

// Debounce delay, matches default, see dtparam -h button_debounce
#define PWR_UP_WAIT_TIME_MS 50
#endif

// custom key codes
enum rpi_keycodes{
    RPI_PWR = QK_KB_0,
};

// Whether to use the debug led
#define WANT_DEBUG_LED 0

// Bit code of any boot keys we're monitoring, lsig takes priority
#define BOOT_KEY_SPACE 0x1
#define BOOT_KEY_SHIFT_LEFT 0x2
#define BOOT_KEY_SHIFT_RIGHT 0x4

// Whether we are logging via the uart
// see UART_TX_PIN in config.h
#if HAL_USE_SIO
#define WANT_UART_LOGGING 1
#endif
