/* Copyright 2024 Raspberry Pi
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

#include "quantum.h"

#include "pi500.h"
#include "usb_main.h" // Used for "USB_DRIVER" to get the USB connection state

#if WANT_UART_LOGGING
#include "uart.h"
#endif

static bool pwr_button_pressed;
static uint8_t boot_key_down; // Special key pressed on boot
static bool usb_active;

#if WANT_UART_LOGGING
static int8_t uart_sendchar(uint8_t c)
{
    uart_write(c);
    // Write a CR after LF
    if (c == 10) {
        uart_write(13);
    }
    return 0;
}
#endif

// The power button is a custom key. To actually activate the power "button" we have to fiddle with a gpio
static void press_power_button(bool press) {
    if (press && !pwr_button_pressed) {
        dprintf("Power button pressed\n");
        setPinOutput(PWR_BTN);
        writePin(PWR_BTN, 1); // 'Press' Power Button
        pwr_button_pressed = true;
    } else if (!press && pwr_button_pressed) {
        dprintf("Power button released\n");
        setPinInput(PWR_BTN); // 'Release' Power Button
        pwr_button_pressed = false;
    }
}

// Runs before the QMK main loop begins
void keyboard_pre_init_kb(void){
    setPinOutput(GP20); // Top Right (Power) Key Column
    writePin(GP20, 1);
    setPinInput(GP6); // Top Right (Power) Key Row
    if (readPin(GP6)) {
        press_power_button(true);
    }
    else {
        press_power_button(false);
    }
    setPinOutput(LED_PIN);
    setPinOutput(CAPS_LED);
    keyboard_pre_init_user();

#if WANT_UART_LOGGING
    uart_init(115200);
    print_set_sendchar(uart_sendchar);
    dprintf("Starting keyboard\n");
#endif
}

// This enables the /dev/hidrawX via USB which requires the hid_listen program to read debug https://www.pjrc.com/teensy/hid_listen.html
// If the uart is enabled it replaces the USB logging and you won't see anything
// dprintf doesn't work without DEBUG_ENABLE defined
#ifdef DEBUG_ENABLE
void keyboard_post_init_user(void) {
    debug_enable = true;
    debug_matrix = true;
}
#endif

static const char *boot_key_name(uint8_t bit_code) {
    switch(bit_code) {
        case BOOT_KEY_SPACE:
            return "space";
        case BOOT_KEY_SHIFT_LEFT:
            return "left shift";
        case BOOT_KEY_SHIFT_RIGHT:
            return "right shift";
        default:
            return "unknown";
    }
}

static void resend_key_down(uint8_t bit_code) {
    dprintf("Resending %s down\n", boot_key_name(bit_code));
    switch(bit_code) {
        case BOOT_KEY_SPACE:
            SEND_STRING(SS_UP(X_SPACE) SS_DOWN(X_SPACE));
            break;
        case BOOT_KEY_SHIFT_LEFT:
            SEND_STRING(SS_UP(X_LEFT_SHIFT) SS_DOWN(X_LEFT_SHIFT));
            break;
        case BOOT_KEY_SHIFT_RIGHT:
            SEND_STRING(SS_UP(X_RIGHT_SHIFT) SS_DOWN(X_RIGHT_SHIFT));
            break;
        default:
            break;
    }
}

// This function is run after QMK is done doing EVERYTHING in one iteration of the main loop
void housekeeping_task_kb(void) {
    static bool done_first_run;

    // Setting the caps lock light on or off
    static bool last_caps_lock;
    if (!done_first_run || host_keyboard_led_state().caps_lock != last_caps_lock) {
        last_caps_lock = host_keyboard_led_state().caps_lock;
        if (last_caps_lock) {
            dprintf("Caps on\n");
            writePin(CAPS_LED, 1);
        }
        else {
            dprintf("Caps off\n");
            writePin(CAPS_LED, 0);
        }
    }
#if WANT_DEBUG_LED
    // DEBUG ONLY
    // Toggle GPIO17. This is a heartbeat and a scan speed indicator
    static bool led_on = false;
    led_on = !led_on;
    if (led_on) {
        writePin(LED_PIN, 1);
    } else {
        writePin(LED_PIN,0);
    }        
#endif
    // Detect whether the host we're connected to seems to be running
    if (!usb_active && USB_DRIVER.state == USB_ACTIVE) {
        usb_active = true;
        dprintf("Device powered up\n");
        // Some keys are used at boot by net install
        // But as the keyboard is powered up before the host it will miss key down events
        // so resend them when we have detected the host is becoming active
        if (boot_key_down) {
            uint8_t bit_code = 0x1;
            while(bit_code) {
                if (boot_key_down & bit_code) {
                    resend_key_down(bit_code);
                    break;
                }
                bit_code <<= 1;
            }
        }
    } else if (usb_active && USB_DRIVER.state != USB_ACTIVE) {
        usb_active = false;
        dprintf("Device powered down\n");
    }
    done_first_run = true; 
}

static void record_boot_key(int key_code, bool pressed) {
    uint32_t bit_code = 0;
    switch(key_code) {
        case KC_SPC:
            bit_code = BOOT_KEY_SPACE;
            break;
        case KC_LSFT:
            bit_code = BOOT_KEY_SHIFT_LEFT;
            break;
        case KC_RSFT:
            bit_code = BOOT_KEY_SHIFT_RIGHT;
            break;
        default:
            return;
    }
    if (pressed) {
        boot_key_down |= bit_code;
    } else {
        boot_key_down &= ~bit_code;
    }
    dprintf("%s %s\n", boot_key_name(bit_code), pressed ? "down" : "up");
}

// This function runs when a key event happens
bool process_record_kb(uint16_t key_code, keyrecord_t *record) { // Custom keycode handling
#if WANT_UART_LOGGING
    if (record->event.pressed) {
        dprintf("Pressed: %u\n", key_code);
    }
#endif
    switch(key_code) {
        case RPI_PWR:
            if (record->event.pressed) {
                dprintf("Power key down usb=%d\n", usb_active);
                press_power_button(true);
            } else {
                dprintf("Power key up usb=%d\n", usb_active);
                press_power_button(false);
            }
            return false;
        case KC_SPC:
        case KC_LSFT:
        case KC_RSFT:
            record_boot_key(key_code, record->event.pressed);
            break;
        default:
            break;
    }
    return process_record_user(key_code, record);
}
