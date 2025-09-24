#ifdef RPI_RGB_STARTUP_ANIMATION
RGB_MATRIX_EFFECT(STARTUP_ANIM_W)
RGB_MATRIX_EFFECT(STARTUP_ANIM_B)

#ifdef RGB_MATRIX_CUSTOM_EFFECT_IMPLS

#include "pi500plus.h"

#define STARTUP_ANIM_FRAMES 120
#if defined(KEYBOARD_rpi_pi500plus_iso)
#	include "startup_anim/Pulse_Black_ISO.h"
#	include "startup_anim/Pulse_White_ISO.h"
#endif
#if defined(KEYBOARD_rpi_pi500plus_ansi)
#	include "startup_anim/Pulse_Black_ANSI.h"
#	include "startup_anim/Pulse_White_ANSI.h"
#endif
#if defined(KEYBOARD_rpi_pi500plus_jis)
#	include "startup_anim/Pulse_Black_JIS.h"
#	include "startup_anim/Pulse_White_JIS.h"
#endif

#define ANIM_INTERVAL 12

static bool STARTUP_ANIM_W(effect_params_t* params) {
  static uint8_t frame;
  static uint32_t wait_timer;
  static bool frame_complete;
  static bool animation_complete;

  RGB_MATRIX_USE_LIMITS(led_min, led_max);

  inline void update_leds(uint8_t new_frame) {
    for (int n = led_min; n < led_max; n++) {
      HSV hsv = {h_values_1[new_frame][n], s_values_1[new_frame][n], scale8(v_values_1[new_frame][n],rgb_matrix_config.hsv.v)};
      RGB rgb = rgb_matrix_hsv_to_rgb(hsv);
      rgb_matrix_set_color(n, rgb.r, rgb.g, rgb.b);
    }
  }

  if (params->init) {
    if (led_min == 0) { // Only run once to save cycles
      // memset(g_rgb_frame_buffer, 0, sizeof g_rgb_frame_buffer);
      frame = 0;
      frame_complete = true;
      animation_complete = false;
      wait_timer = g_rgb_timer + ANIM_INTERVAL;
    }
  }

  if (!animation_complete) {
    if (!frame_complete) {
      update_leds(frame);
      if (!rgb_matrix_check_finished_leds(led_max)) {
        frame++;
        wait_timer = g_rgb_timer + ANIM_INTERVAL;
        frame_complete = true;
        return false;
      }
      else {
        return true;
      }
    }
    else if (g_rgb_timer > wait_timer) {
      if (frame<STARTUP_ANIM_FRAMES) {
        frame_complete = false;
        update_leds(frame);
        if (!rgb_matrix_check_finished_leds(led_max)) {
          frame++;
          wait_timer = g_rgb_timer + ANIM_INTERVAL;
          frame_complete = true;
          return false;
        }
        else {
          return true;
        }
      }
      else {
        animation_complete = true;
        dprintf("Animation Complete - Return to EEPROM stored RGB mode\n");
        rpi_rgb_matrix_startup_callback(true);
        return false;
      }
    }
  }

  return rgb_matrix_check_finished_leds(led_max); // Returns false if led_max is the final led in the matrix
}

static bool STARTUP_ANIM_B(effect_params_t* params) {
  static uint8_t frame;
  static uint32_t wait_timer;
  static bool frame_complete;
  static bool animation_complete;

  RGB_MATRIX_USE_LIMITS(led_min, led_max);

  inline void update_leds(uint8_t new_frame) {
    for (int n = led_min; n < led_max; n++) {
      HSV hsv = {h_values_0[new_frame][n], s_values_0[new_frame][n], scale8(v_values_0[new_frame][n],rgb_matrix_config.hsv.v)};
      RGB rgb = rgb_matrix_hsv_to_rgb(hsv);
      rgb_matrix_set_color(n, rgb.r, rgb.g, rgb.b);
    }
  }

  if (params->init) {
    if (led_min == 0) { // Only run once to save cycles
      // memset(g_rgb_frame_buffer, 0, sizeof g_rgb_frame_buffer);
      frame = 0;
      frame_complete = true;
      animation_complete = false;
      wait_timer = g_rgb_timer + ANIM_INTERVAL;
    }
  }

  if (!animation_complete) {
    if (!frame_complete) {
      update_leds(frame);
      if (!rgb_matrix_check_finished_leds(led_max)) {
        frame++;
        wait_timer = g_rgb_timer + ANIM_INTERVAL;
        frame_complete = true;
        return false;
      }
      else {
        return true;
      }
    }
    else if (g_rgb_timer > wait_timer) {
      if (frame<STARTUP_ANIM_FRAMES) {
        frame_complete = false;
        update_leds(frame);
        if (!rgb_matrix_check_finished_leds(led_max)) {
          frame++;
          wait_timer = g_rgb_timer + ANIM_INTERVAL;
          frame_complete = true;
          return false;
        }
        else {
          return true;
        }
      }
      else {
        animation_complete = true;
        dprintf("Animation Complete - Return to EEPROM stored RGB mode\n");
        rpi_rgb_matrix_startup_callback(false);
        return false;
      }
    }
  }

  return rgb_matrix_check_finished_leds(led_max); // Returns false if led_max is the final led in the matrix
}

#endif // #ifdef RGB_MATRIX_CUSTOM_EFFECT_IMPLS
#endif // #ifdef RPI_RGB_STARTUP_ANIMATION