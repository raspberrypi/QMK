/* Copyright 2025 Raspberry Pi
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

#ifdef RGB_MATRIX_ENABLE
// I2C LED Driver (IS31FL3729) Driver addresses, LED Counts
#    define DRIVER_ADDR_1 0x34
#    define DRIVER_ADDR_2 0x35
#    define DRIVER_COUNT 2
#    define DRIVER_1_LED_TOTAL 41
#    define DRIVER_2_LED_TOTAL 43
#    define RGB_MATRIX_LED_COUNT (DRIVER_1_LED_TOTAL + DRIVER_2_LED_TOTAL)

// QMK Index of Caps Lock key and Activity LED (Under Power Key)
#    define CAPS_LOCK_LED_INDEX 46
#    define ACTIVITY_LED_INDEX 15
#endif