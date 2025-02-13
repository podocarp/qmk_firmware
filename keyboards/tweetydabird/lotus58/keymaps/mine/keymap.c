// Copyright 2022 Markus Knutsson (@TweetyDaBird)
// SPDX-License-Identifier: GPL-2.0-or-later
#include QMK_KEYBOARD_H
#include "ocean_dream.h"

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {[0] = LAYOUT(QK_GESC, KC_1, KC_2, KC_3, KC_4, KC_5, KC_MPLY, KC_MPLY, KC_6, KC_7, KC_8, KC_9, KC_0, KC_MINS, KC_TAB, KC_Q, KC_W, KC_E, KC_R, KC_T, KC_Y, KC_U, KC_I, KC_O, KC_P, KC_BSLS, KC_LSFT, KC_A, KC_S, KC_D, KC_F, KC_G, KC_H, KC_J, KC_K, KC_L, KC_SCLN, KC_QUOT, KC_LCTL, KC_Z, KC_X, KC_C, KC_V, KC_B, MO(3), MO(3), KC_N, KC_M, KC_COMM, KC_DOT, KC_SLSH, KC_EQL, KC_LGUI, MO(1), KC_LALT, KC_SPC, SC_SENT, KC_BSPC, MO(1), MO(2)),
                                                              [1] = LAYOUT(KC_TRNS, KC_F1, KC_F2, KC_F3, KC_F4, KC_F5, KC_TRNS, KC_TRNS, KC_F6, KC_F7, KC_F8, KC_F9, KC_F10, KC_F11, KC_TILD, KC_EXLM, KC_AT, KC_HASH, KC_DLR, KC_PERC, KC_CIRC, KC_AMPR, KC_ASTR, KC_LPRN, KC_RPRN, KC_F12, KC_GRV, KC_1, KC_2, KC_3, KC_4, KC_5, KC_6, KC_7, KC_8, KC_9, KC_0, KC_PEQL, KC_LPRN, KC_RPRN, KC_LBRC, KC_RBRC, KC_LCBR, KC_RCBR, KC_TRNS, KC_TRNS, KC_LT, KC_GT, KC_PPLS, KC_PMNS, KC_PAST, KC_PSLS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_DEL, KC_TRNS, KC_TRNS),
                                                              [2] = LAYOUT(KC_MPLY, KC_MRWD, KC_MFFD, KC_VOLU, KC_VOLD, KC_MUTE, KC_TRNS, KC_TRNS, KC_SLEP, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, MS_BTN1, MS_BTN2, MS_BTN3, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, MS_LEFT, MS_DOWN, MS_UP, MS_RGHT, KC_LEFT, KC_DOWN, KC_UP, KC_RGHT, KC_TRNS, KC_TRNS, KC_TRNS, MS_ACL0, MS_ACL1, MS_ACL2, MS_WHLU, MS_WHLD, KC_TRNS, EE_CLR, KC_HOME, KC_PGUP, KC_PGDN, KC_END, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_PSCR, QK_BOOT, KC_TRNS, KC_TRNS, KC_TRNS),
                                                              [3] = LAYOUT(KC_6, KC_7, KC_8, KC_9, KC_0, KC_MINS, KC_TRNS, KC_TRNS, KC_ESC, KC_1, KC_2, KC_3, KC_4, KC_5, KC_Y, KC_U, KC_I, KC_O, KC_P, KC_BSLS, KC_TAB, KC_Q, KC_W, KC_E, KC_R, KC_T, KC_H, KC_J, KC_K, KC_L, KC_SCLN, KC_QUOT, KC_LSFT, KC_A, KC_S, KC_D, KC_F, KC_G, KC_N, KC_M, KC_COMM, KC_DOT, KC_SLSH, KC_EQL, KC_TRNS, KC_TRNS, KC_LCTL, KC_Z, KC_X, KC_C, KC_V, KC_B, KC_RSFT, KC_BSPC, KC_RCTL, KC_ENT, KC_TRNS, KC_DEL, KC_TRNS, KC_TRNS)};

#if defined(ENCODER_MAP_ENABLE)
const uint16_t PROGMEM encoder_map[][NUM_ENCODERS][NUM_DIRECTIONS] = {[0] = {ENCODER_CCW_CW(KC_VOLD, KC_VOLU), ENCODER_CCW_CW(KC_VOLD, KC_VOLU)}, [1] = {ENCODER_CCW_CW(KC_TRNS, KC_TRNS), ENCODER_CCW_CW(KC_TRNS, KC_TRNS)}, [2] = {ENCODER_CCW_CW(KC_TRNS, KC_TRNS), ENCODER_CCW_CW(KC_TRNS, KC_TRNS)}, [3] = {ENCODER_CCW_CW(KC_TRNS, KC_TRNS), ENCODER_CCW_CW(KC_TRNS, KC_TRNS)}};
#endif

#ifdef OLED_ENABLE
/*
static void render_anim(void) {
    if (first_render) {
        oled_write_raw_P(frame, ANIM_SIZE);
        first_render = 0;
    } else {
        change_frame_bytewise(c_frame);
    }
    c_frame = c_frame + 1 > IDLE_FRAMES ? 0 : c_frame + 1;
}
*/

bool oled_task_user(void) {
    // Render the OLED
    // render_anim();
    render_stars();
    return false;
}

#endif

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case KC_LCTL:
        case KC_RCTL:
#ifdef OCEAN_DREAM_ENABLE
            is_calm = (record->event.pressed) ? true : false;
#endif
            break;
    }
    return true;
}
