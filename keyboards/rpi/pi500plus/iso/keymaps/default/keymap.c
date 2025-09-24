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

#include QMK_KEYBOARD_H
#include "sendstring_uk.h"


enum layers {
    BASE,
	_FN
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

	/*
     * ┌───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┐
     * │Esc│F1 │F2 │F3 │F4 │F5 │F6 │F7 │F8 │F9 │F10│F11│F12│PSc│Del│Pwr│
     * ├───┼───┼───┼───┼───┼───┼───┼───┼───┼───┼───┼───┼───┼───┴───┼───┤
     * │ ` │ 1 │ 2 │ 3 │ 4 │ 5 │ 6 │ 7 │ 8 │ 9 │ 0 │ - │ = │ Backsp│PgU│
     * ├───┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─────┼───┤
     * │ Tab │ Q │ W │ E │ R │ T │ Y │ U │ I │ O │ P │ [ │ ] │     │PgD│
     * ├─────┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┬──┴┐ Ent├───┤
     * │ Caps │ A │ S │ D │ F │ G │ H │ J │ K │ L │ ; │ ' │ # │    │Hme│
     * ├────┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴─┬─┴───┴┬───┼───┤
     * │Shft│ \ │ Z │ X │ C │ V │ B │ N │ M │ , │ . │ / │Shift │ ↑ │End│
     * ├────┼───┴┬──┴─┬─┴───┴───┴───┴───┴───┴──┬┴──┬┴──┬┴──┬───┼───┼───┤
     * │Ctrl│GUI │Alt │                        │Alt│Fn │Ctl│ ← │ ↓ │ → │
     * └────┴────┴────┴────────────────────────┴───┴───┴───┴───┴───┴───┘
     */

		[BASE] = LAYOUT(
			KC_ESC,  KC_F1,   KC_F2, KC_F3, KC_F4, KC_F5, KC_F6, KC_F7, KC_F8, KC_F9,   KC_F10,  KC_F11,  KC_F12,  KC_PSCR, KC_DEL,  
			UK_GRV,  UK_1,    UK_2,  UK_3,  UK_4,  UK_5,  UK_6,  UK_7,  UK_8,  UK_9,    UK_0,    UK_MINS, UK_EQL,           KC_BSPC, KC_PGUP,
			KC_TAB,  		  UK_Q,  UK_W,  UK_E,  UK_R,  UK_T,  UK_Y,  UK_U,  UK_I,    UK_O,    UK_P,    UK_LBRC, UK_RBRC,          KC_PGDN,
			KC_CAPS, 	      UK_A,  UK_S,  UK_D,  UK_F,  UK_G,  UK_H,  UK_J,  UK_K,    UK_L,    UK_SCLN, UK_QUOT, UK_HASH, KC_ENT,  KC_HOME,
			KC_LSFT, UK_BSLS, UK_Z,  UK_X,  UK_C,  UK_V,  UK_B,  UK_N,  UK_M,  UK_COMM, UK_DOT,  UK_SLSH,          KC_RSFT, KC_UP,   KC_END,
			KC_LCTL, KC_LGUI, KC_LALT,                    KC_SPC,                       KC_RIGHT_ALT , MO(_FN), KC_RCTL, KC_LEFT, KC_DOWN, KC_RGHT),
		
		[_FN] = LAYOUT(
			_______, _______, _______, RPI_RGB_HUI, RPI_RGB_MOD, RGB_VAD, RGB_VAI, _______, _______, _______, KC_AUDIO_MUTE, KC_AUDIO_VOL_DOWN, KC_AUDIO_VOL_UP, _______, KC_INS,
			_______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,          _______, _______,
			_______, 		  _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,         _______,
			_______, 		  _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, 
			_______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,          _______, _______,  _______, 
			_______, _______, _______,                            _______,                            _______, _______, _______, _______, _______, _______),
};

