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

// I2C Definitions (I2C also must be enabled in halconf.h and mcuconf.h)
#define I2C1_SDA_PIN GP12
#define I2C1_SCL_PIN GP13
#define I2C_DRIVER I2CD0

// #define UART_TX_PIN GP28 // Debug UART TX Pin (requires board mod)

// Disable double tap to reseet:
#define RP2040_BOOTLOADER_DOUBLE_TAP_RESET_TIMEOUT 0

// RGB Matrix Definitions:
#ifdef RGB_MATRIX_ENABLE
// Custom Defines for IS31FL3729 Behaviour here, otherwise defaults are set in is31fl3729.c
// Scaling values adjusted to improve white when PWM = 255, 255, 255
#    define ISSI_SCALING_BLUE 73
#    define ISSI_SCALING_RED 217
#    define ISSI_SCALING_GREEN 240
//#    define ISSI_SCALING_GREEN 0xff - This default is already set
//#    define ISSI_GLOBAL_CURRENT 64 - This default is already set
//#    define ISSI_PWM_FREQUENCY ISSI_PWM_32_KHZ - This default is already set
//#    define ISSI_PDR_PUR_SETTING 0b00110011 - This default is already set

#    define RGB_MATRIX_KEYPRESSES // Enables RGB Matrix Reactive (on keypress) Effects
#    define RGB_MATRIX_FRAMEBUFFER_EFFECTS // Enables RGB Matrix Framebuffer Effects
#    define RGB_MATRIX_LED_PROCESS_LIMIT 4 // Limits the number of LEDs to process in an animation per task run (increases keyboard responsiveness)
#    define RGB_MATRIX_LED_FLUSH_LIMIT 16 // Limits in milliseconds how frequently an animation will update the LEDs. 16 (16ms) is equivalent to limiting to 60fps (increases keyboard responsiveness)
#    define RGB_TRIGGER_ON_KEYDOWN  

#    define RGB_MATRIX_DEFAULT_MODE RGB_MATRIX_SOLID_COLOR // Default RGB pattern if none stored in EEPROM
#    define RGB_MATRIX_DEFAULT_ON true // Sets the default enabled state, if none stored in EEPROM
#    define RGB_MATRIX_DEFAULT_HUE 0 // Sets the default hue value, if none stored in EEPROM
#    define RGB_MATRIX_DEFAULT_SAT 255 // Sets the default saturation value, if none stored in EEPROM
#    define RGB_MATRIX_DEFAULT_VAL 255 // Sets the default brightness value, if none stored in EEPROM
#    define RGB_MATRIX_DEFAULT_SPD 127 // Sets the default animation speed, if none stored in EEPROM
#    define RGB_MATRIX_DEFAULT_FLAGS LED_FLAG_NONE

#    define RPI_ACTIVITY_LED // If defined, enables bicolour Power/Activity LED
#    define RPI_INDICATORS_MIN_BRIGHTNESS 100 // Indicator LEDs (Caps/Activity) follow overall keyboard brightness down to this minimum point
#    define RPI_RGB_DISABLE_AUTO // If defined, RGB pattern is disabled when the mainboard power is off (i.e. Red activity LED on)
#    define RPI_RGB_DISABLE_TIMER 50 // Minimum ticks to wait between detecting change in red led status and toggling RGB
#    define RPI_RGB_STARTUP_ANIMATION // If defined, custom startup animation is run during RGB matrix enable (during 500+ boot)

// RGB Matrix Animation modes. Explicitly enabled
// For full list of effects, see:
// https://docs.qmk.fm/#/feature_rgb_matrix?id=rgb-matrix-effects
// None and Solid Colour Defined By Default

//#    define ENABLE_RGB_MATRIX_ALPHAS_MODS //2
//#    define ENABLE_RGB_MATRIX_GRADIENT_UP_DOWN
#    define ENABLE_RGB_MATRIX_GRADIENT_LEFT_RIGHT
//#    define ENABLE_RGB_MATRIX_BREATHING
//#    define ENABLE_RGB_MATRIX_BAND_SAT
//#    define ENABLE_RGB_MATRIX_BAND_VAL
//#    define ENABLE_RGB_MATRIX_BAND_PINWHEEL_SAT
//#    define ENABLE_RGB_MATRIX_BAND_PINWHEEL_VAL
//#    define ENABLE_RGB_MATRIX_BAND_SPIRAL_SAT //10
//#    define ENABLE_RGB_MATRIX_BAND_SPIRAL_VAL
//#    define ENABLE_RGB_MATRIX_CYCLE_ALL
//#    define ENABLE_RGB_MATRIX_CYCLE_LEFT_RIGHT 
//#    define ENABLE_RGB_MATRIX_CYCLE_UP_DOWN
//#    define ENABLE_RGB_MATRIX_RAINBOW_MOVING_CHEVRON
//#    define ENABLE_RGB_MATRIX_CYCLE_OUT_IN
//#    define ENABLE_RGB_MATRIX_CYCLE_OUT_IN_DUAL
#    define ENABLE_RGB_MATRIX_CYCLE_PINWHEEL
//#    define ENABLE_RGB_MATRIX_CYCLE_SPIRAL
//#    define ENABLE_RGB_MATRIX_DUAL_BEACON //20
//#    define ENABLE_RGB_MATRIX_RAINBOW_BEACON
//#    define ENABLE_RGB_MATRIX_RAINBOW_PINWHEELS
//#    define ENABLE_RGB_MATRIX_FLOWER_BLOOMING
//#    define ENABLE_RGB_MATRIX_RAINDROPS
//#    define ENABLE_RGB_MATRIX_JELLYBEAN_RAINDROPS
//#    define ENABLE_RGB_MATRIX_HUE_BREATHING
//#    define ENABLE_RGB_MATRIX_HUE_PENDULUM
//#    define ENABLE_RGB_MATRIX_HUE_WAVE
//#    define ENABLE_RGB_MATRIX_PIXEL_RAIN
//#    define ENABLE_RGB_MATRIX_PIXEL_FLOW //30
//#    define ENABLE_RGB_MATRIX_PIXEL_FRACTAL //31
// Below enabled only if RGB_MATRIX_FRAMEBUFFER_EFFECTS is defined
#    define ENABLE_RGB_MATRIX_TYPING_HEATMAP //32
//#    define ENABLE_RGB_MATRIX_DIGITAL_RAIN // Overwrites activity LED
// Below enabled only of RGB_MATRIX_KEYPRESSES or RGB_MATRIX_KEYRELEASES is defined
#    define ENABLE_RGB_MATRIX_SOLID_REACTIVE_SIMPLE
//#    define ENABLE_RGB_MATRIX_SOLID_REACTIVE
//#    define ENABLE_RGB_MATRIX_SOLID_REACTIVE_WIDE
#//    define ENABLE_RGB_MATRIX_SOLID_REACTIVE_MULTIWIDE
//#    define ENABLE_RGB_MATRIX_SOLID_REACTIVE_CROSS
//#    define ENABLE_RGB_MATRIX_SOLID_REACTIVE_MULTICROSS
//#    define ENABLE_RGB_MATRIX_SOLID_REACTIVE_NEXUS
//#    define ENABLE_RGB_MATRIX_SOLID_REACTIVE_MULTINEXUS
//#    define ENABLE_RGB_MATRIX_SPLASH
//#    define ENABLE_RGB_MATRIX_MULTISPLASH
//#    define ENABLE_RGB_MATRIX_SOLID_SPLASH
//#    define ENABLE_RGB_MATRIX_SOLID_MULTISPLASH
//#    define ENABLE_RGB_MATRIX_STARLIGHT // Overwrites activity LED 
//#    define ENABLE_RGB_MATRIX_STARLIGHT_DUAL_HUE
//#    define ENABLE_RGB_MATRIX_STARLIGHT_DUAL_SAT
//#    define ENABLE_RGB_MATRIX_RIVERFLOW
#endif