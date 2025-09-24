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

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "progmem.h"

typedef struct is31_led {
    uint8_t driver : 2;
    uint8_t r;
    uint8_t g;
    uint8_t b;
} __attribute__((packed)) is31_led;

extern const is31_led PROGMEM g_is31_leds[RGB_MATRIX_LED_COUNT];

void IS31FL3729_driver_init(uint8_t addr);
void IS31FL3729_write_register(uint8_t addr, uint8_t reg, uint8_t data);
void IS31FL3729_write_pwm_buffer(uint8_t addr, uint8_t *pwm_buffer);

void IS31FL3729_set_color(int index, uint8_t red, uint8_t green, uint8_t blue);
void IS31FL3729_set_color_all(uint8_t red, uint8_t green, uint8_t blue);

void IS31FL3729_set_led_control_register(uint8_t index, bool red, bool green, bool blue);

// This should not be called from an interrupt
// (eg. from a timer interrupt).
// Call this while idle (in between matrix scans).
// If the buffer is dirty, it will update the driver with the buffer.
void IS31FL3729_update_pwm_buffers(uint8_t addr, uint8_t index);
void IS31FL3729_update_led_control_registers(uint8_t addr, uint8_t index);

// IS31FL3729 Registers:

#define ISSI_REG_PWM_1 0x01 // PWM Registers go from 01h ~ 8Fh (for 15x9)
#define ISSI_REG_SCALING_1 0x90 // Scaling Registers go from 90h ~ 9Fh
#define ISSI_PAGE_OPEN_SHORT_1 0xB3 // Open/Short Registers go from B3h ~ C4h

#define ISSI_REG_CONFIGURATION 0xA0
#define ISSI_REG_GLOBALCURRENT 0xA1
#define ISSI_REG_PULLS 0xB0
#define ISSI_REG_SPREAD_SPECTRUM 0xB1
#define ISSI_REG_PWM_FREQUENCY 0xB2
#define ISSI_REG_RESET 0xCF

// PWM Frequencies:
#define ISSI_PWM_55_KHZ 0b000
#define ISSI_PWM_32_KHZ 0b001
#define ISSI_PWM_4_KHZ 0b010
#define ISSI_PWM_2_KHZ 0b011
#define ISSI_PWM_1_KHZ 0b100
#define ISSI_PWM_80_KHZ 0b111

// PWM Register Map:

#define SW1_CS1 0x01
#define SW1_CS2 0x02
#define SW1_CS3 0x03
#define SW1_CS4 0x04
#define SW1_CS5 0x05
#define SW1_CS6 0x06
#define SW1_CS7 0x07
#define SW1_CS8 0x08
#define SW1_CS9 0x09
#define SW1_CS10 0x0A
#define SW1_CS11 0x0B
#define SW1_CS12 0x0C
#define SW1_CS13 0x0D
#define SW1_CS14 0x0E
#define SW1_CS15 0x0F

#define SW2_CS1 0x11
#define SW2_CS2 0x12
#define SW2_CS3 0x13
#define SW2_CS4 0x14
#define SW2_CS5 0x15
#define SW2_CS6 0x16
#define SW2_CS7 0x17
#define SW2_CS8 0x18
#define SW2_CS9 0x19
#define SW2_CS10 0x1A
#define SW2_CS11 0x1B
#define SW2_CS12 0x1C
#define SW2_CS13 0x1D
#define SW2_CS14 0x1E
#define SW2_CS15 0x1F

#define SW3_CS1 0x21
#define SW3_CS2 0x22
#define SW3_CS3 0x23
#define SW3_CS4 0x24
#define SW3_CS5 0x25
#define SW3_CS6 0x26
#define SW3_CS7 0x27
#define SW3_CS8 0x28
#define SW3_CS9 0x29
#define SW3_CS10 0x2A
#define SW3_CS11 0x2B
#define SW3_CS12 0x2C
#define SW3_CS13 0x2D
#define SW3_CS14 0x2E
#define SW3_CS15 0x2F

#define SW4_CS1 0x31
#define SW4_CS2 0x32
#define SW4_CS3 0x33
#define SW4_CS4 0x34
#define SW4_CS5 0x35
#define SW4_CS6 0x36
#define SW4_CS7 0x37
#define SW4_CS8 0x38
#define SW4_CS9 0x39
#define SW4_CS10 0x3A
#define SW4_CS11 0x3B
#define SW4_CS12 0x3C
#define SW4_CS13 0x3D
#define SW4_CS14 0x3E
#define SW4_CS15 0x3F

#define SW5_CS1 0x41
#define SW5_CS2 0x42
#define SW5_CS3 0x43
#define SW5_CS4 0x44
#define SW5_CS5 0x45
#define SW5_CS6 0x46
#define SW5_CS7 0x47
#define SW5_CS8 0x48
#define SW5_CS9 0x49
#define SW5_CS10 0x4A
#define SW5_CS11 0x4B
#define SW5_CS12 0x4C
#define SW5_CS13 0x4D
#define SW5_CS14 0x4E
#define SW5_CS15 0x4F

#define SW6_CS1 0x51
#define SW6_CS2 0x52
#define SW6_CS3 0x53
#define SW6_CS4 0x54
#define SW6_CS5 0x55
#define SW6_CS6 0x56
#define SW6_CS7 0x57
#define SW6_CS8 0x58
#define SW6_CS9 0x59
#define SW6_CS10 0x5A
#define SW6_CS11 0x5B
#define SW6_CS12 0x5C
#define SW6_CS13 0x5D
#define SW6_CS14 0x5E
#define SW6_CS15 0x5F

#define SW7_CS1 0x61
#define SW7_CS2 0x62
#define SW7_CS3 0x63
#define SW7_CS4 0x64
#define SW7_CS5 0x65
#define SW7_CS6 0x66
#define SW7_CS7 0x67
#define SW7_CS8 0x68
#define SW7_CS9 0x69
#define SW7_CS10 0x6A
#define SW7_CS11 0x6B
#define SW7_CS12 0x6C
#define SW7_CS13 0x6D
#define SW7_CS14 0x6E
#define SW7_CS15 0x6F

#define SW8_CS1 0x71
#define SW8_CS2 0x72
#define SW8_CS3 0x73
#define SW8_CS4 0x74
#define SW8_CS5 0x75
#define SW8_CS6 0x76
#define SW8_CS7 0x77
#define SW8_CS8 0x78
#define SW8_CS9 0x79
#define SW8_CS10 0x7A
#define SW8_CS11 0x7B
#define SW8_CS12 0x7C
#define SW8_CS13 0x7D
#define SW8_CS14 0x7E
#define SW8_CS15 0x7F

#define SW9_CS1 0x81
#define SW9_CS2 0x82
#define SW9_CS3 0x83
#define SW9_CS4 0x84
#define SW9_CS5 0x85
#define SW9_CS6 0x86
#define SW9_CS7 0x87
#define SW9_CS8 0x88
#define SW9_CS9 0x89
#define SW9_CS10 0x8A
#define SW9_CS11 0x8B
#define SW9_CS12 0x8C
#define SW9_CS13 0x8D
#define SW9_CS14 0x8E
#define SW9_CS15 0x8F