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

/*Notes on Driver Implementation: 
- Only Supports for 15x9 Matrix Mode
- Maximum 2 drivers
*/

#include "is31fl3729.h"
#include "pi500plus.h"
#include "quantum.h"
#include "i2c_master.h"
#include "wait.h"
#include "rgb_matrix.h"

// This is a 7-bit address, that gets left-shifted and bit 0
// set to 0 for write, 1 for read (as per I2C protocol)
// A2:A1 of the address will vary depending on your wiring:
// 00 <-> GND
// 01 <-> SCL
// 10 <-> SDA
// 11 <-> VCC
// The result is 0b01101(A2:A1)
#define ISSI_ADDR_DEFAULT 0x34

#ifndef ISSI_TIMEOUT
#    define ISSI_TIMEOUT 10 // How long to  wait for i2c messagages, in milliseconds
#endif

#ifndef ISSI_PERSISTENCE
#    define ISSI_PERSISTENCE 0 // Retry failed messages this many times
#endif

#ifndef ISSI_GLOBALCURRENT
#    define ISSI_GLOBALCURRENT 64 // Only 64 Global Current Steps on IS31FL3729
#endif

#ifndef ISSI_SCALING_BLUE
#    define ISSI_SCALING_BLUE 0xff // Current Scaling for Blue CSx
#endif

#ifndef ISSI_SCALING_GREEN
#    define ISSI_SCALING_GREEN 0xff // Current Scaling for Green CSx
#endif

#ifndef ISSI_SCALING_RED
#    define ISSI_SCALING_RED 0xff // Current Scaling for Red CSx
#endif

#ifndef ISSI_PWM_FREQUENCY
#   define ISSI_PWM_FREQUENCY ISSI_PWM_32_KHZ
#endif

#ifndef ISSI_SPREAD_SPECTRUM_SETTING
#   define ISSI_SPREAD_SPECTRUM_SETTING 0x0 // Spread Spectrum Off
#endif

#ifndef ISSI_PUR_PDR_SETTING // Register setting for Pull Down/Up Resistors
#   define ISSI_PUR_PDR_SETTING 0b00110011 // Default (0 deg phase delay, 2.0kΩ only in SWx and CSx off time)
#endif

// Transfer buffer for TWITransmitData()
uint8_t g_twi_transfer_buffer[20];

// The PWM buffers match the IS31FL3729 PWM registers.
// Storing them like this is optimal for I2C transfers to the registers.
// We could optimize this and take out the unused registers from these
// buffers and the transfers in IS31FL3729_write_pwm_buffer() but it's
// probably not worth the extra complexity.

uint8_t g_pwm_buffer[DRIVER_COUNT][144]; // 16*9 LEDs (15x9 usable as LED driver also supports 16x8)
bool    g_pwm_buffer_update_required[DRIVER_COUNT] = {false};

void IS31FL3729_write_register(uint8_t addr, uint8_t reg, uint8_t data) {
    g_twi_transfer_buffer[0] = reg;
    g_twi_transfer_buffer[1] = data;

#if ISSI_PERSISTENCE > 0
    for (uint8_t i = 0; i < ISSI_PERSISTENCE; i++) {
        if (i2c_transmit(addr << 1, g_twi_transfer_buffer, 2, ISSI_TIMEOUT) == 0) break;
    }
#else
    i2c_transmit(addr << 1, g_twi_transfer_buffer, 2, ISSI_TIMEOUT);
#endif
}

void IS31FL3729_write_pwm_buffer(uint8_t addr, uint8_t *pwm_buffer) {
    // Transmit PWM registers in 9 transfers of 15 bytes (CS1->CS15)
    // g_twi_transfer_buffer[] is 20 bytes
    // Addresses autoincrement but need to follow pattern 0x01 -> 0x0F, 0x11 -> 0x1F, etc. 
    // Therefore reset register address every 15 transfers to address for (SWi_CS1)

    for (int i = 0; i < 9; i += 1) {
        g_twi_transfer_buffer[0] = ISSI_REG_PWM_1 + (i*0x10); //0x01, 0x11, 0x21, etc.
        
        // Copy 15 bytes of data from pwm buffer to transfer buffer
        memcpy(g_twi_transfer_buffer + 1, (pwm_buffer + (i*16)), 15);

#if ISSI_PERSISTENCE > 0
        for (uint8_t i = 0; i < ISSI_PERSISTENCE; i++) {
            if (i2c_transmit(addr << 1, g_twi_transfer_buffer, 16, ISSI_TIMEOUT) == 0) break;
        }
#else
        i2c_transmit(addr << 1, g_twi_transfer_buffer, 16, ISSI_TIMEOUT);
#endif
    }
}

void IS31FL3729_driver_init(uint8_t addr) {
    // Clear the PWM registers, setup mode and other settings,
    // then disable software shutdown.

    // Clear PWM Registers:
    for (int i = 0x01; i <= 0x8F; i++) {
        IS31FL3729_write_register(addr, i, 0x00);
    }

    // Scaling Registers - each colour has separate 256-bit current scaling value
    // BLUE: CS1, CS4, CS7, CS10, CS13 -> 0x90, 0x93, 0x96, 0x99, 0x9C
    // GREEN: CS2, CS5, CS8, CS11, CS14 -> 0x91, 0x94, 0x97, 0x9A, 0x9D
    // RED: CS3, CS6, CS9, CS12, CS15 -> 0x92, 0x95, 0x98, 0x9B, 0x9E
    for (int i = 0; i< 5; i++) {
        IS31FL3729_write_register(addr, (ISSI_REG_SCALING_1+(3*i)), ISSI_SCALING_BLUE);
        IS31FL3729_write_register(addr, (ISSI_REG_SCALING_1+(3*i)+1), ISSI_SCALING_GREEN);
        IS31FL3729_write_register(addr, (ISSI_REG_SCALING_1+(3*i)+2), ISSI_SCALING_RED);
    }

    // Set Global Current Register:
    IS31FL3729_write_register(addr, ISSI_REG_GLOBALCURRENT, ISSI_GLOBALCURRENT);

    // Set Pull Down/Up Resistors Register:
    // D7 -> Phase Choice (0x0 = 0 Degree Delay, 0x1 = 180 Degree Delay)
    // D6:D4 -> SWx Pull Down Resistor Selection (0b011 = 2.0kΩ only in SWx off time, default)
    // D3 -> N/A
    // D2:D0 -> CSy Pull Up Resistor Selection (0b011 = 2.0kΩ only in CSy off time, default)
    IS31FL3729_write_register(addr, ISSI_REG_PULLS, ISSI_PUR_PDR_SETTING);

    // Set Spread Spectrum Register (default off):
    IS31FL3729_write_register(addr, ISSI_REG_SPREAD_SPECTRUM, ISSI_SPREAD_SPECTRUM_SETTING);

    // Set PWM Frequency Register
    IS31FL3729_write_register(addr, ISSI_REG_PWM_1, ISSI_PWM_FREQUENCY);

    // Config Register:
    // D7:D4 -> SWx Setting. 0x0000 is 15x9 matrix, which this driver is written for.
    // D3 -> H/L Logic. Default to 0x0 (1.4/0.4V)
    // D2:D1 -> Open Short Detection Enable. 0x00 is disabled.
    // D0 -> Software Shutdown Control. 0x0 is software shutdown, 0x1 is normal operation.
    IS31FL3729_write_register(addr, ISSI_REG_CONFIGURATION, 0x1);
}

static void IS31FL3729_init(void) {
    // Toggle LED Shutdown in case SDA is stuck in bad state
    writePin(LED_SHUTDOWN, 1);
    wait_us(10);
    writePin(LED_SHUTDOWN, 0);

    i2c_init();

    IS31FL3729_driver_init(DRIVER_ADDR_1);
    #if defined(DRIVER_ADDR_2)
    IS31FL3729_driver_init(DRIVER_ADDR_2);
    #endif

    //From Datasheet: There should be no I2C operation 10us before and after SDB rising edge
    wait_ms(1); // Wait 1ms to ensure last I2C operation has finished

    writePin(LED_SHUTDOWN, 1); // Disable Hardware Shutdown (Current Sources Enabled, Registers Always Active)
    wait_ms(1); // Wait 1ms to ensure the device(s) have woken up.
}

void IS31FL3729_update_pwm_buffers(uint8_t addr, uint8_t index) {
    if (g_pwm_buffer_update_required[index]) {
        IS31FL3729_write_pwm_buffer(addr, g_pwm_buffer[index]);
    }
    g_pwm_buffer_update_required[index] = false;
}

static void IS31FL3729_flush(void) {
    IS31FL3729_update_pwm_buffers(DRIVER_ADDR_1, 0);
    #if defined(DRIVER_ADDR_2)
    IS31FL3729_update_pwm_buffers(DRIVER_ADDR_2, 1);
    #endif
}

void IS31FL3729_set_color(int index, uint8_t red, uint8_t green, uint8_t blue) {
    is31_led led;
    if (index >= 0 && index < RGB_MATRIX_LED_COUNT) {
        memcpy_P(&led, (&g_is31_leds[index]), sizeof(led));

        g_pwm_buffer[led.driver][led.r - 1]          = red; //-1 as first LED is at 0x01
        g_pwm_buffer[led.driver][led.g - 1]          = green;
        g_pwm_buffer[led.driver][led.b - 1]          = blue;
        g_pwm_buffer_update_required[led.driver] = true;
    }
}

void IS31FL3729_set_color_all(uint8_t red, uint8_t green, uint8_t blue) {
    for (int i = 0; i < RGB_MATRIX_LED_COUNT; i++) {
        IS31FL3729_set_color(i, red, green, blue);
    }
}

// Link IC-specific functions to generic rgb matrix driver:
const rgb_matrix_driver_t rgb_matrix_driver = {
    .init = IS31FL3729_init,
    .flush = IS31FL3729_flush,
    .set_color = IS31FL3729_set_color,
    .set_color_all = IS31FL3729_set_color_all,
};