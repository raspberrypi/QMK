// Copyright 2023 QMK
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H
#include "sendstring_japanese.h" // Contains the Japanese keycodes


enum layers {
    BASE,
    _FN
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

		[BASE] = LAYOUT(
            KC_Q,       KC_W,       KC_E,       KC_R,       KC_U,       KC_I,       KC_O,       _______,    _______,    _______,    _______,    _______,        KC_P,       KC_INT3,    _______,    _______,    
            KC_TAB,     KC_CAPS,    KC_F3,      KC_T,       KC_Y,       JP_LBRC,    KC_F7,      _______,    _______,    KC_LGUI,    _______,    _______,        JP_AT,      KC_BSPC,    KC_LSFT,    _______,
            KC_A,       KC_S,       KC_D,       KC_F,       KC_J,       KC_K,       KC_L,       _______,    KC_INT4,    _______,    _______,    MO(_FN),        JP_SCLN,    KC_BSLS,    KC_RSFT,    _______,
            KC_ESC,     KC_NUBS,    KC_F4,      KC_G,       KC_H,       KC_F6,      _______,    KC_UP,      KC_INT2,    _______,    _______,    KC_LALT,        JP_COLN,    _______,    _______,    KC_SPC,
            KC_Z,       KC_X,       KC_C,       KC_V,       KC_M,       KC_COMM,    KC_DOT,     _______,    KC_INT5,    _______,    KC_RCTL,    _______,        KC_BSLS,    KC_ENT,     _______,    KC_PSCR,
            KC_INT5,    KC_INT4,    KC_INT2,    KC_B,       KC_N,       KC_INT1,    KC_APP,     KC_LEFT,    KC_RGHT,    _______,    _______,    KC_RIGHT_ALT,   KC_SLSH,    JP_BSLS,    _______,    KC_DOWN,
            KC_GRV,     KC_F1,      KC_F2,      KC_5,       KC_6,       JP_CIRC,    KC_F8,      _______,    _______,    _______,    KC_LCTL,    _______,        KC_MINS,    KC_F9,      _______,    RPI_PWR,
            KC_1,       KC_2,       KC_3,       KC_4,       KC_7,       KC_8,       KC_9,       KC_INT3,    _______,    _______,    KC_F5,      KC_DEL,         KC_0,       KC_F10,     _______,    _______),

		
		[_FN] = LAYOUT(
            _______,    _______,    _______,    _______,    _______,    _______,    _______,    _______,    _______,    _______,    _______,    _______,     _______,    _______,    _______,    _______,
            _______,    _______,    _______,    _______,    _______,    _______,    _______,    _______,    _______,    _______,    _______,    _______,     _______,    _______,    _______,    _______,
            _______,    _______,    _______,    _______,    _______,    _______,    _______,    _______,    _______,    _______,    _______,    _______,     _______,    _______,    _______,    _______,
            _______,    _______,    _______,    _______,    _______,    _______,    _______,    KC_PGUP,    _______,    _______,    _______,    _______,     _______,    _______,    _______,    _______,
            _______,    _______,    _______,    _______,    _______,    _______,    _______,    _______,    _______,    _______,    _______,    _______,     _______,    _______,    _______,    _______,
            _______,    _______,    _______,    _______,    _______,    _______,    _______,    KC_HOME,     KC_END,    _______,    _______,    _______,     _______,    _______,    _______,    KC_PGDN,
            _______,     KC_F11,     KC_F12,    _______,    _______,    _______,    _______,    _______,    _______,    _______,    _______,    _______,     _______,    _______,    _______,    _______,
            _______,    _______,    _______,    _______,    _______,    _______,    _______,    _______,    _______,    _______,    _______,     KC_INS,     _______,    _______,    _______,    _______)
};
