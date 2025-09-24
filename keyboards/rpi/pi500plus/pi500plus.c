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
 
#include "quantum.h"
#include "pi500plus.h"
#include "is31fl3729.h"
#include "usb_main.h" // Used for "USB_DRIVER" to get the USB connection state
#include "eeconfig.h"

#ifdef WANT_UART_LOGGING
#include "uart.h"
#endif

static uint8_t boot_key_down; // Special key pressed on boot
static bool usb_active;

#ifdef WANT_UART_LOGGING
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

#ifdef RGB_MATRIX_ENABLE
// Struct for EEPROM storage of RGB mode:
typedef union {
  uint32_t raw;
  struct {
    uint8_t     rpi_rgb_mode_active :8;
    uint8_t     rpi_rgb_h :8;
  };
} kb_config_t;

kb_config_t kb_config;

const rpi_rgb_mode_t rpi_rgb_modes[RPI_RGB_MODES_MAX] = {
	{RGB_MATRIX_DEFAULT_FLAGS, RGB_MATRIX_DEFAULT_MODE, RGB_MATRIX_DEFAULT_SPD, true, false, RGB_MATRIX_DEFAULT_HUE, RGB_MATRIX_DEFAULT_SAT}, // Mode 0 matches RGB default, for simplicity when EEPROM is reset
    {LED_FLAG_ALL, RGB_MATRIX_SOLID_COLOR, RGB_MATRIX_DEFAULT_SPD, true, true, 0, 0}, 
    {LED_FLAG_ALL, RGB_MATRIX_SOLID_COLOR, RGB_MATRIX_DEFAULT_SPD, false, true, 0, 255},
    {LED_FLAG_ALL, RGB_MATRIX_GRADIENT_LEFT_RIGHT, 127, true, true, 0, 255},
    {LED_FLAG_ALL, RGB_MATRIX_CYCLE_PINWHEEL, 80, true, true, 0, 255},
    {LED_FLAG_ALL, RGB_MATRIX_TYPING_HEATMAP, 150, true, false, 0, 255},
    {LED_FLAG_ALL, RGB_MATRIX_SOLID_REACTIVE_SIMPLE, 100, false, false, 0, 255}
};

void change_rpi_rgb_mode(uint8_t new_mode) {
    // Functionality note: hue/sat/mode/speed changes made externally (e.g. with VIA/raw HID) will be lost when using RPI_RGB_MOD
    rgb_matrix_set_flags(rpi_rgb_modes[new_mode].flags);
    rgb_matrix_mode(rpi_rgb_modes[new_mode].effect);
    rgb_matrix_set_speed(rpi_rgb_modes[new_mode].speed);
    if (rpi_rgb_modes[new_mode].fixed_hue) {
        rgb_matrix_sethsv(rpi_rgb_modes[new_mode].h, rpi_rgb_modes[new_mode].s, rgb_matrix_get_val());
    }
    else {
        kb_config.raw = eeconfig_read_kb();
        rgb_matrix_sethsv(kb_config.rpi_rgb_h, rpi_rgb_modes[new_mode].s, rgb_matrix_get_val());
    }
    kb_config.rpi_rgb_mode_active = new_mode; // Store update in EEPROM
    eeconfig_update_kb(kb_config.raw);
    dprintf("RGB Mode Updated to: %i\n", kb_config.rpi_rgb_mode_active);
}

void change_rpi_rgb_hue(bool increase) {
    if (increase) {
        rgb_matrix_increase_hue();
    }
    else {
        rgb_matrix_decrease_hue();
    }
    kb_config.rpi_rgb_h = rgb_matrix_get_hue();
    eeconfig_update_kb(kb_config.raw); // Store hue value in EEPROM
    dprintf("RPI Hue updated to %i\n", kb_config.rpi_rgb_h);
}

#endif // RGB_MATRIX_ENABLE

void keyboard_pre_init_kb(void) {
    // Need to set debug options here for UART logging pre-keyboard init:
#ifdef DEBUG_ENABLE
    debug_enable=true;
#endif
#ifdef DEBUG_KEYBOARD_ENABLE
    debug_keyboard=true;
#endif
#ifdef DEBUG_MATRIX_ENABLE
    debug_matrix=true;
#endif
    // Setup Pi500+-specific pins:
    setPinInput(GRN_LED); // 2712_STAT_LED
    setPinInput(RED_LED); // RP1_STAT_LED
    setPinOutput(LED_SHUTDOWN); // LED Driver Shutdown Pin
    setPinOutput(LED_PIN); // Debug LED Pin
    writePin(LED_SHUTDOWN, 0); // Default to Shutdown, RGB Matrix init will pull high to wake.
#ifdef WANT_UART_LOGGING
    uart_init(115200);
    print_set_sendchar(uart_sendchar);
    print("Starting keyboard\n");
#endif
    keyboard_pre_init_user(); // Continue QMK Init
}

#ifdef RGB_MATRIX_ENABLE

void eeconfig_init_kb(void) { // If EEPROM is reset then reset kb EEPROM storage (only used for active RPI RGB mode)
    kb_config.rpi_rgb_mode_active = 0; // Mode 0 is the same as the default values for the RGB Matrix subsystem
    kb_config.rpi_rgb_h = 0; // Default Hue = 0 (Red)
    eeconfig_update_kb(kb_config.raw);
}

#ifdef RPI_RGB_DISABLE_AUTO
bool mainboard_on = false;
uint32_t power_led_timer;
bool power_led_timer_running;
bool startup_running = false;

#ifdef RPI_RGB_STARTUP_ANIMATION
#define SAT_FADE_TIMER_DELAY 5
#define SAT_FADE_STEP 2
uint32_t sat_fade_timer;
uint8_t sat_fade_target;
bool sat_fade_timer_running = false;

#define VAL_FADE_TIMER_DELAY 5
#define VAL_FADE_STEP 2
uint32_t val_fade_timer;
uint8_t val_fade_target;
bool val_fade_timer_running = false;

void rpi_rgb_matrix_startup_callback(bool startup_animation_type) {
    dprintf("Startup Callback\n");
    if(mainboard_on) { // Check mainboard hasn't powered off during startup animation
        rgb_matrix_reload_from_eeprom();
        memset(g_rgb_frame_buffer, 0, sizeof g_rgb_frame_buffer); // Ensures heatmap resets after startup animation
        if(startup_animation_type && rgb_matrix_config.hsv.s!=0) { // Saturation Fade
            sat_fade_target = rgb_matrix_config.hsv.s;
            rgb_matrix_config.hsv.s = 0;
            dprintf("rgb_matrix_config.hsv.s = %d \n", rgb_matrix_config.hsv.s);
            sat_fade_timer = sync_timer_read32() + SAT_FADE_TIMER_DELAY;
            sat_fade_timer_running = true;
            dprintf("Sat Fade Begun\n");
        }
        else if(!startup_animation_type && rgb_matrix_get_mode()!=rpi_rgb_modes[kb_config.rpi_rgb_mode_active].effect) { // Undefined behaviour to val fade up to animation
            val_fade_target = rgb_matrix_config.hsv.v;
            rgb_matrix_config.hsv.v = 0;
            dprintf("rgb_matrix_config.hsv.v = %d \n", rgb_matrix_config.hsv.v);
            val_fade_timer = sync_timer_read32() + VAL_FADE_TIMER_DELAY;
            val_fade_timer_running = true;
            dprintf("Val Fade Begun\n");
        }
        else {
            startup_running = false;
        }
    }
    else {
        rgb_matrix_reload_from_eeprom();
        rgb_matrix_set_flags_noeeprom(LED_FLAG_NONE);
        startup_running = false;
    }
}
#endif // RPI_RGB_STARTUP_ANIMATION

void rpi_rgb_matrix_startup(void) {
    rgb_matrix_reload_from_eeprom();
    #ifdef RPI_RGB_STARTUP_ANIMATION
    if(mainboard_on && !startup_running) {
        startup_running = true;
        rgb_matrix_set_flags_noeeprom(LED_FLAG_ALL); // Enable RGB flags for animation
        // Active RPI RGB mode local copy updated in eeconfig_init so don't need to re-read
        if (rgb_matrix_get_mode()!=rpi_rgb_modes[kb_config.rpi_rgb_mode_active].effect) { // If RGB mode is 'unknown' then run safest startup animation
            // rgb_matrix_mode_noeeprom(RGB_MATRIX_CUSTOM_STARTUP_ANIM_B);
            rgb_matrix_config.mode = RGB_MATRIX_CUSTOM_STARTUP_ANIM_B;
        }
        else if(rpi_rgb_modes[kb_config.rpi_rgb_mode_active].startup_animation) { // Some animations look better ending on a white background
            //rgb_matrix_mode_noeeprom(RGB_MATRIX_CUSTOM_STARTUP_ANIM_W);
            rgb_matrix_config.mode = RGB_MATRIX_CUSTOM_STARTUP_ANIM_W;
        } else { // Some animations look better on a black background
            // rgb_matrix_mode_noeeprom(RGB_MATRIX_CUSTOM_STARTUP_ANIM_B);
            rgb_matrix_config.mode = RGB_MATRIX_CUSTOM_STARTUP_ANIM_B;
        }
        dprintf("Startup Animation Started\n");
        // Startup animation callback function called at end of animation
    }
    #endif // RPI_RGB_STARTUP_ANIMATION
}
#endif // RPI_RGB_DISABLE_AUTO
#endif // RGB_MATRIX_ENABLE

void keyboard_post_init_kb(void) {
#ifdef DEBUG_ENABLE
    debug_enable=true;
#endif
#ifdef DEBUG_KEYBOARD_ENABLE
    debug_keyboard=true;
#endif
#ifdef DEBUG_MATRIX_ENABLE
    debug_matrix=true;
#endif
#ifdef RGB_MATRIX_ENABLE
    kb_config.raw = eeconfig_read_kb(); // Ensure local copy of kb_config is updated
#ifdef RPI_RGB_DISABLE_AUTO
    rgb_matrix_set_flags_noeeprom(LED_FLAG_NONE);
#endif // RPI_RGB_DISABLE_AUTO
#endif // RGB_MATRIX_ENABLE
}

#ifdef CONSOLE_ENABLE
static const char *boot_key_name(uint8_t bit_code) { // Needs to be dependent on console enable
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
#endif

static void resend_key_down(uint8_t bit_code) {
    #ifdef CONSOLE_ENABLE
    dprintf("Resending %s down\n", boot_key_name(bit_code));
    #endif
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


void housekeeping_task_kb(void) {
    #ifdef DEBUG_LED_ENABLE
    // DEBUG ONLY
    // Toggle GPIO15. This is a heartbeat and a scan speed indicator
    static bool led_on = false;
    led_on = !led_on;
    if (led_on) {
        writePin(LED_PIN, 1);
    } else {
        writePin(LED_PIN,0);
    }        
    #endif //#ifdef DEBUG_LED_ENABLE

    // Detect whether the host we're connected to seems to be running
    if (!usb_active && USB_DRIVER.state == USB_ACTIVE) {
        usb_active = true;
        dprintf("USB Active\n");
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
        #ifdef RGB_MATRIX_ENABLE
        if (!mainboard_on) {
            mainboard_on = true;
            rpi_rgb_matrix_startup(); // Reenable stored rgb state with startup animation
        }
        #endif // #ifdef RGB_MATRIX_ENABLE
    } else if (usb_active && USB_DRIVER.state != USB_ACTIVE) {
        usb_active = false;
        dprintf("USB Down\n");
    }

    #ifdef RGB_MATRIX_ENABLE
    #ifdef RPI_RGB_DISABLE_AUTO
    bool mainboard_status = readPin(RED_LED); // RP1_STAT_LED (active low)
    if (power_led_timer_running) { // If timer is running, wait until it has complete before rechecking status and committing to status change
        if (sync_timer_read32() > power_led_timer) {
            if (!mainboard_status && mainboard_on) {
                mainboard_on = false;
                dprintf("RED LED on (Device powered off)\n");
                if (!startup_running) { // Startup animation functions will turn off animation cleanly
                    rgb_matrix_set_flags_noeeprom(LED_FLAG_NONE); // Disable RGB (apart from indicators)
                }
            }
            power_led_timer_running = false;
        }
    }
    else {
        if (!mainboard_status && mainboard_on) {
            power_led_timer = sync_timer_read32() + RPI_RGB_DISABLE_TIMER;
            power_led_timer_running = true;
            dprintf("RED LED Timer Started\n");
        }
    }
    #ifdef RPI_RGB_STARTUP_ANIMATION
    if(sat_fade_timer_running) {
        if (sync_timer_read32() > sat_fade_timer) {
            // Might be a nicer way to do this calculation but qadd8 would not include properly
            uint16_t i = rgb_matrix_config.hsv.s + SAT_FADE_STEP;
            if (i>255) {
                rgb_matrix_config.hsv.s = 255;
            } else {
                rgb_matrix_config.hsv.s = i;
            }
            dprintf("rgb_matrix_config.hsv.s = %d \n", rgb_matrix_config.hsv.s);
            if(rgb_matrix_config.hsv.s == sat_fade_target) {
                sat_fade_timer_running = false;
                if (!mainboard_on) {
                    rgb_matrix_set_flags_noeeprom(LED_FLAG_NONE);
                }
                startup_running = false;
                dprintf("Sat Fade Complete\n");
            }
            else {
                sat_fade_timer = sync_timer_read32() + SAT_FADE_TIMER_DELAY;
            }
        }
    }
    if(val_fade_timer_running) {
        if (sync_timer_read32() > val_fade_timer) {
            // Might be a nicer way to do this calculation but qadd8 would not include properly
            uint16_t i = rgb_matrix_config.hsv.v + VAL_FADE_STEP;
            if (i>val_fade_target) {
                rgb_matrix_config.hsv.v = val_fade_target;
            } else {
                rgb_matrix_config.hsv.v = i;
            }
            dprintf("rgb_matrix_config.hsv.v = %d \n", rgb_matrix_config.hsv.v);
            if(rgb_matrix_config.hsv.v == val_fade_target) {
                val_fade_timer_running = false;
                if (!mainboard_on) {
                    rgb_matrix_set_flags_noeeprom(LED_FLAG_NONE);
                }
                startup_running = false;
                dprintf("Val Fade Complete\n");
            }
            else {
                val_fade_timer = sync_timer_read32() + VAL_FADE_TIMER_DELAY;
            }
        }
    }
    #endif // RPI_RGB_STARTUP_ANIMATION
    #endif // RPI_RGB_DISABLE_AUTO
    #endif // RGB_MATRIX_ENABLE
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

#ifndef RPI_RGB_DISABLE_AUTO
bool mainboard_on = true;
#endif

bool process_record_kb(uint16_t key_code, keyrecord_t *record) { // Custom keycode handling
    switch(key_code) {
        #ifdef RGB_MATRIX_ENABLE    
        // Change Toggle behaviour so indicator LEDs cannot be disabled:
        case RGB_TOG:
            if (record->event.pressed && mainboard_on && !startup_running) {
                switch (rgb_matrix_get_flags()) {
                    case LED_FLAG_ALL: {
                        rgb_matrix_set_flags(LED_FLAG_NONE); // Stores status to EEPROM
                    } break;
                    default: {
                        rgb_matrix_set_flags(LED_FLAG_ALL); // Stores status to EEPROM
                    } break;
                }
            }
            return false;
        // Disable all default RGB Keycodes when mainboard off
        case RGB_MOD:
            if (mainboard_on && !startup_running) {
                return true;
            }
            else {
                return false;
            }
        case RGB_RMOD:
            if (mainboard_on && !startup_running) {
                return true;
            }
            else {
                return false;
            }
        case RGB_HUD:
            if (mainboard_on && !startup_running) {
                return true; 
            }
            else {
                return false;
            }
        case RGB_HUI:
            if (mainboard_on && !startup_running) {
                return true;
            }
            else {
                return false;
            }
        case RGB_SAI:
            if (mainboard_on && !startup_running) {
                return true;
            }
            else {
                return false;
            }
        case RGB_SAD:
            if (mainboard_on && !startup_running) {
                return true;
            }
            else {
                return false;
            }
         case RGB_VAI:
            if (mainboard_on && !startup_running) {
                return true;
            }
            else {
                return false;
            }
         case RGB_VAD:
            if (mainboard_on && !startup_running) {
                return true;
            }
            else {
                return false;
            }
         case RGB_SPI:
            if (mainboard_on && !startup_running) {
                return true;
            }
            else {
                return false;
            }
         case RGB_SPD:
            if (mainboard_on && !startup_running) {
                return true;
            }
            else {
                return false;
            }
        // Custom RGB Keycode behaviour:
        case RPI_RGB_MOD:
            if (record->event.pressed && mainboard_on && !startup_running) {
                kb_config.raw = eeconfig_read_kb();
                uint8_t is_shifted = get_mods() & MOD_MASK_SHIFT;
                if(is_shifted) { // Decrease RGB Mode
                    if (kb_config.rpi_rgb_mode_active<1) {
                        change_rpi_rgb_mode((RPI_RGB_MODES_MAX-1));
                    }
                    else {
                        change_rpi_rgb_mode((kb_config.rpi_rgb_mode_active-1));
                    }
                }
                else { // Increase RGB Mode
                    if (kb_config.rpi_rgb_mode_active==(RPI_RGB_MODES_MAX-1)) {
                        change_rpi_rgb_mode(0);
                    }
                    else {
                        change_rpi_rgb_mode((kb_config.rpi_rgb_mode_active+1));
                    }
                }
            }
            return false;
        case RPI_RGB_RMOD:
            if (record->event.pressed && mainboard_on && !startup_running) {
                kb_config.raw = eeconfig_read_kb();
                uint8_t is_shifted = get_mods() & MOD_MASK_SHIFT;
                if(is_shifted) { // Decrease RGB Mode
                    if (kb_config.rpi_rgb_mode_active==(RPI_RGB_MODES_MAX-1)) {
                        change_rpi_rgb_mode(0);
                    }
                    else {
                        change_rpi_rgb_mode((kb_config.rpi_rgb_mode_active+1));
                    }
                }
                else { // Increase RGB Mode
                    
                    if (kb_config.rpi_rgb_mode_active<1) {
                        change_rpi_rgb_mode((RPI_RGB_MODES_MAX-1));
                    }
                    else {
                        change_rpi_rgb_mode((kb_config.rpi_rgb_mode_active-1));
                    }
                }
            }
            return false;
        case RPI_RGB_HUI:
            if (record->event.pressed && mainboard_on && !startup_running && !rpi_rgb_modes[kb_config.rpi_rgb_mode_active].fixed_hue) { // Allow hue change only if mainboard active & RGB mode does not have fixed hue
                uint8_t is_shifted = get_mods() & MOD_MASK_SHIFT;
                if(is_shifted) { // Decrease RGB Hue
                    change_rpi_rgb_hue(false);
                }
                else { // Increase RGB Hue
                    change_rpi_rgb_hue(true);
                }
            }
            return false; 
        case RPI_RGB_HUD:
            if (record->event.pressed && mainboard_on && !startup_running && !rpi_rgb_modes[kb_config.rpi_rgb_mode_active].fixed_hue) { // Allow hue change only if mainboard active & RGB mode does not have fixed hue
                uint8_t is_shifted = get_mods() & MOD_MASK_SHIFT;
                if(is_shifted) { // Increase RGB Hue
                    change_rpi_rgb_hue(true);
                }
                else { // Decrease RGB Hue
                    change_rpi_rgb_hue(false);
                }
            }
            return false;
        #endif //#ifdef RGB_MATRIX_ENABLE
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

#ifdef RGB_MATRIX_ENABLE
bool rgb_matrix_indicators_advanced_kb(uint8_t led_min, uint8_t led_max) {
    inline uint8_t brightness(void) {
        if(rgb_matrix_get_val()<RPI_INDICATORS_MIN_BRIGHTNESS) {
            return RPI_INDICATORS_MIN_BRIGHTNESS;
        }
        else {
            return rgb_matrix_get_val();
        }
    }

    #ifdef RPI_ACTIVITY_LED
    bool green_led_status = readPin(GRN_LED); // 2712_STAT_LED (active high)
    bool red_led_status = readPin(RED_LED); // RP1_STAT_LED (active low)
    if (green_led_status && red_led_status) {
        RGB_MATRIX_INDICATOR_SET_COLOR(ACTIVITY_LED_INDEX, 0, brightness(), 0); 
    }
    else if (green_led_status) {
        RGB_MATRIX_INDICATOR_SET_COLOR(ACTIVITY_LED_INDEX, brightness(), brightness(), 0);
    }
    else if (red_led_status) {
        RGB_MATRIX_INDICATOR_SET_COLOR(ACTIVITY_LED_INDEX, 0, 0, 0);
    }
    else {
        RGB_MATRIX_INDICATOR_SET_COLOR(ACTIVITY_LED_INDEX, brightness(), 0, 0);
    }
    #endif // #ifdef RPI_ACTIVITY_LED
    #ifdef CAPS_LOCK_LED_INDEX
    if(!startup_running) {
        if (rgb_matrix_get_sat()<100) { // Where saturation is low (keys close to white), CAPS will light up red instead of white for visibility
            if (host_keyboard_led_state().caps_lock) {
                RGB_MATRIX_INDICATOR_SET_COLOR(CAPS_LOCK_LED_INDEX, brightness(), 0, 0);
            } else {
                if (!rgb_matrix_get_flags()) {
                RGB_MATRIX_INDICATOR_SET_COLOR(CAPS_LOCK_LED_INDEX, 0, 0, 0);
                }
            }
        }
        else {
            if (host_keyboard_led_state().caps_lock) {
                RGB_MATRIX_INDICATOR_SET_COLOR(CAPS_LOCK_LED_INDEX, brightness(), brightness(), brightness());
            } else {
                if (!rgb_matrix_get_flags()) {
                RGB_MATRIX_INDICATOR_SET_COLOR(CAPS_LOCK_LED_INDEX, 0, 0, 0);
                }
            }
        }
    }
    #endif // #ifdef CAPS_LOCK_LED_INDEX
    return true;
}

#endif //#ifdef RGB_MATRIX_ENABLE