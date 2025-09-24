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

#include "quantum.h"

// GPIO Definitions for Pi 500+ specific GPIO
#define RED_LED GP29 // RP1 Status LED (Active Low)
#define GRN_LED GP28 // 2712 Status LED (Active High)
#define LED_SHUTDOWN GP14 // Controls both LED Drivers, Shutdown when = 0
#define LED_PIN GP15 // Spare pin for debug LED connection

enum rpi_keycodes {
	RPI_RGB_MOD = QK_KB_0, // Custom RGB Mode keycode
	RPI_RGB_RMOD, // Custom RGB Mode keycode (reverse)
	RPI_RGB_HUI, // Custom RGB Hue increase keycode
	RPI_RGB_HUD // Custom RGB Hue decrease keycode
};

#ifdef RGB_MATRIX_ENABLE

#ifdef RPI_RGB_STARTUP_ANIMATION
void rpi_rgb_matrix_startup_callback(bool startup_animation_type);
#endif //#ifdef RPI_RGB_STARTUP_ANIMATION


enum rpi_rgb_modes_enum {
	RPI_DEFAULT = 0,
	RPI_WHITE,
	RPI_SOLID_COLOUR,
	RPI_STATIC_GRADIENT,
	RPI_CYCLE_PINWHEEL,
	RPI_TYPING_HEATMAP,
	RPI_REACTIVE_SIMPLE,
	RPI_RGB_MODES_MAX
};

// Struct for each mode containing all necessary variables to fully define RGB behaviour in that mode (brightness is always independently controlled):
// Flags = 255 is animation active, Flags = 0 is all leds off (apart from indicators)
// Effect, Speed, Hue, Saturation (all uint8_t)
// Fixed hue - whether hue control keycodes are active for this animation
// Startup animation - what version of the startup animation to use - true for ending on LEDs all white - false for ending on LEDs off
typedef struct {
	uint8_t flags;
	uint8_t effect;
	uint8_t speed;
	bool fixed_hue;
	bool startup_animation;
	uint8_t h;
	uint8_t s;
} rpi_rgb_mode_t;

#endif //#ifdef RGB_MATRIX_ENABLE

// Whether to use the debug led
//#define DEBUG_LED_ENABLE

// Other Debug Options
// #define DEBUG_ENABLE
// #define DEBUG_KEYBOARD_ENABLE
// #define DEBUG_MATRIX_ENABLE

// Whether we are logging via the uart
//#define WANT_UART_LOGGING

// Bit code of any boot keys we're monitoring, lsig takes priority
#define BOOT_KEY_SPACE 0x1
#define BOOT_KEY_SHIFT_LEFT 0x2
#define BOOT_KEY_SHIFT_RIGHT 0x4