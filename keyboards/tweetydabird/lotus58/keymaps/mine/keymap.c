// Copyright 2022 Markus Knutsson (@TweetyDaBird)
// SPDX-License-Identifier: GPL-2.0-or-later
#include QMK_KEYBOARD_H
#include "../../ocean_dream.h"
#include "out.h"

const keypos_t PROGMEM hand_swap_config[MATRIX_ROWS][MATRIX_COLS] = {
    {{0, 5}, {1, 5}, {2, 5}, {3, 5}, {4, 5}, {5, 5}}, // Left hand
    {{0, 6}, {1, 6}, {2, 6}, {3, 6}, {4, 6}, {5, 6}}, //
    {{0, 7}, {1, 7}, {2, 7}, {3, 7}, {4, 7}, {5, 7}}, //
    {{0, 8}, {1, 8}, {2, 8}, {3, 8}, {4, 8}, {5, 8}}, //
    {{0, 9}, {1, 9}, {2, 9}, {3, 9}, {4, 9}, {5, 9}}, //

    {{0, 0}, {1, 0}, {2, 0}, {3, 0}, {4, 0}, {5, 0}}, // Right hand
    {{0, 1}, {1, 1}, {2, 1}, {3, 1}, {4, 1}, {5, 1}}, //
    {{0, 2}, {1, 2}, {2, 2}, {3, 2}, {4, 2}, {5, 2}}, //
    {{0, 3}, {1, 3}, {2, 3}, {3, 3}, {4, 3}, {5, 3}}, //
    {{0, 4}, {1, 4}, {2, 4}, {3, 4}, {4, 4}, {5, 4}}, //
};
const keypos_t PROGMEM encoder_hand_swap_config[1] = {{0, 0}};

#if defined(ENCODER_MAP_ENABLE)
const uint16_t PROGMEM encoder_map[][NUM_ENCODERS][NUM_DIRECTIONS] = {
    [0] = {ENCODER_CCW_CW(KC_VOLD, KC_VOLU), ENCODER_CCW_CW(KC_VOLD, KC_VOLU)}, //
    [1] = {ENCODER_CCW_CW(MS_WHLD, MS_WHLU), ENCODER_CCW_CW(MS_WHLU, MS_WHLD)}, //
    [2] = {ENCODER_CCW_CW(MS_WHLL, MS_WHLR), ENCODER_CCW_CW(MS_WHLR, MS_WHLL)}, //
    [3] = {ENCODER_CCW_CW(KC_TRNS, KC_TRNS), ENCODER_CCW_CW(KC_TRNS, KC_TRNS)}  //
};
#endif

#ifdef OLED_ENABLE
bool oled_task_user(void) {
    // Render the OLED
    render_stars();
    return false;
}

#endif

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
#ifdef OCEAN_DREAM_ENABLE
        case KC_LCTL:
        case KC_RCTL:
            is_calm = (record->event.pressed) ? true : false;
            break;
#endif
    }
    return true;
}
