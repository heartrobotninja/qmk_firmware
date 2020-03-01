#include QMK_KEYBOARD_H
#include "action_layer.h"
#include "action_util.h"
#include "debug.h"
#include "eeconfig.h"
#include "version.h"
#include "wait.h"

/* Aliases */
#define ____ KC_TRNS

/* Layers */
enum
{
  COLE = 0,
  QWERTY, // QWERTY-rific
  NUM, // right hand 10 key
  BFV,  // Battlefield V specific layout on top of GAME
  FORT, // Fortnite specific layout on top of GAME
  AUX,   // Things like rebooting the board to be flashed. NUM + RAISE
};

/* Case Identifier for Accents */
enum
{
  LOWER = 0,
  UPPER,
};

/* Language Identifier for Accents */
enum
{
  DE = 0,
  ES,
};

/* Macros */
enum
{
  NONE = 0,

  // OS Functions
  F_PASTE,

  // RGB Macro
  RGB_ANI,
};

/* Tap Dancery */
enum
{
  TD_COPY,
  TD_UNDO,
  TD_FIND,
};

/* OS Identifier */
enum
{
  OS_WIN = 0,
  OS_OSX,
  OS_LIN,
};

uint8_t os_type = OS_WIN;
uint8_t lang = DE;

uint8_t prev_layer;
static uint16_t layer_timer = 0;
static uint16_t rgb_timer;
bool time_travel = false;
bool skip_leds = false;
bool led_on = false;

/* Keymaps */

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

        /* Keymap 0: Colemak Layer
         *
         * ,-----------------------------------------------------.           ,-----------------------------------------------------.
         * |    ` ~    | 1  ! | 2  @ | 3  # | 4  $ | 5  % | UNDO |           | FIND | 6  ^ | 7  & | 8  * | 9  ( | 0  ) |   - _     |
         * |           |      |      |      |      |      |      |           |      |      |      |      |      |      |           |
         * |-----------+------+------+------+------+------+------|           |------+------+------+------+------+------+-----------|
         * |    ' "    |   Q   |   W  |   F  |   P  |  G  | COPY |           | LOCK |   J  |   L  | U Ü  |   Y  | = +  |   ; :     |
         * |-----------+------+------+------+------+------|      |           |      |------+------+------+------+------+-----------|
         * |    [ {    | A  Ä  |   R  | S  ß |   T  |  D  |------|           |------|   H  |   N  |   E  |   I  | O Ö  |   ] }     |
         * |-----------+------+------+------+------+------| PASTE|           | LEAD |------+------+------+------+------+-----------|
         * |    , <    |  Z   |   X  |   C  |   V  |   B  |      |           |      |   K  |   M  |  / ? | \ |  |   ^  |   . >     |
         * `-----------+------+------+------+------+-------------'           `-------------+------+------+------+------+-----------'
         *     |  NUM  |QWERTY| AUX  | MEH  | PGUP |                                       | PGDN | HYPR |   <  |   v  |   >   |
         *     `-----------------------------------'                                       `-----------------------------------'
         *                                         ,-------------.           ,-------------.
         *                                         | CTRL | ALT  |           | HOME |  END |
         *                                  ,------|------|------|           |------+------+------.
         *                                  |      |      | Del  |           | ESC  |      |      |
         *                                  | Space|Backsp|------|           |------| Enter| TAB  |
         *                                  |      |      | LS ( |           | RS ) |      |      |
         *                                  `--------------------'           `--------------------'
         */

        [COLE] = LAYOUT_ergodox(
            // Left Hand
            KC_GRAVE, KC_1, KC_2, KC_3, KC_4, KC_5, TD(TD_UNDO),
            KC_QUOTE, KC_Q, KC_W, KC_F, KC_P, KC_G, TD(TD_COPY),
            KC_LBRACKET, KC_A, KC_R, KC_S, KC_T, KC_D,
            KC_COMMA, KC_Z, KC_X, KC_C, KC_V, KC_B, LCTL(KC_V),
            TT(NUM), TG(QWERTY), TT(AUX), KC_MEH, KC_PGUP,
            OSM(MOD_LCTL), OSM(MOD_LALT),
            KC_DELETE,
            KC_SPC, KC_BSPC, KC_LSPO,

            // Right Hand
            TD(TD_FIND), KC_6, KC_7, KC_8, KC_9, KC_0, KC_MINUS,
            LGUI(KC_L), KC_J, KC_L, KC_U, KC_Y, KC_EQL, KC_SCOLON,
            KC_H, KC_N, KC_E, KC_I, KC_O, KC_RBRACKET,
            KC_LEAD, KC_K, KC_M, KC_SLSH, KC_BSLS, KC_UP, KC_DOT,
            KC_PGDOWN, KC_HYPR, KC_LEFT, KC_DOWN, KC_RGHT,
            KC_HOME, KC_END,
            KC_ESC,
            KC_RSPC, KC_ENT, KC_TAB),

         /* Keymap 2: QWERTY Layer
         *
         * ,-----------------------------------------------------.           ,-----------------------------------------------------.
         * |    ` ~    | 1  ! | 2  @ | 3  # | 4  $ | 5  % | UNDO |           | FIND | 6  ^ | 7  & | 8  * | 9  ( | 0  ) |   - _     |
         * |           |      |      |      |      |      |      |           |      |      |      |      |      |      |           |
         * |-----------+------+------+------+------+------+------|           |------+------+------+------+------+------+-----------|
         * |   TAB     |   Q  |   W  |   E  |   R  |   T  | COPY |           | LOCK |   Y  |   U  |   I  |   O  |   P  |   = +     |
         * |-----------+------+------+------+------+------|      |           |      |------+------+------+------+------+-----------|
         * |   [ {     |   A  |   S  |   D  |   F  |   G  |------|           |------|   H  |   J  |   K  |   L  | ; :  |   ' "     |
         * |-----------+------+------+------+------+------|      |           |      |------+------+------+------+------+-----------|
         * |   ] }     |   Z  |   X  |   C  |   V  |   B  | PASTE|           | LEAD |   N  |   M  |  , < | . >  | / ?  |   \ |     |
         * `-----------+------+------+------+------+-------------'           `-------------+------+------+------+------+-----------'
         *     |  NUM  | ---- |  BFV | FORT | PGUP |                                       | PGDN |   ^  |   v  |   <  |   >  |
         *     `-----------------------------------'                                       `----------------------------------'
         *                                         ,-------------.           ,-------------.
         *                                         | CTRL |  ALT |           | HOME |  END |
         *                                  ,------|------|------|           |------+------+------.
         *                                  |      |      |  DEL |           | ESC  |      |      |
         *                                  | Space|Backsp|------|           |------| Enter| TAB  |
         *                                  |      |      | LSFT |           | RSFT |      |      |
         *                                  `--------------------'           `--------------------'
         */
        [QWERTY] = LAYOUT_ergodox(
            // Left Hand
            KC_GRAVE, KC_1, KC_2, KC_3, KC_4, KC_5, TD(TD_UNDO),
            KC_TAB, KC_Q, KC_W, KC_E, KC_R, KC_T, TD(TD_COPY),
            KC_LBRACKET, KC_A, KC_S, KC_D, KC_F, KC_G,
            KC_RBRACKET, KC_Z, KC_X, KC_C, KC_V, KC_B, LCTL(KC_V),
            TT(NUM), ____, TG(BFV), TG(FORT), KC_PGUP,
            KC_LCTL, KC_LALT,
            KC_DELETE,
            KC_SPC, KC_BSPC, KC_LSHIFT,

            // Right Hand
            TD(TD_FIND), KC_6, KC_7, KC_8, KC_9, KC_0, KC_MINUS,
            LGUI(KC_L), KC_Y, KC_U, KC_I, KC_O, KC_P, KC_EQL,
            KC_H, KC_J, KC_K, KC_L, KC_SCOLON, KC_QUOTE,
            KC_LEAD, KC_N, KC_M, KC_COMMA, KC_DOT, KC_SLSH, KC_BSLS,
            KC_PGDOWN, KC_UP, KC_DOWN, KC_LEFT, KC_RGHT,
            KC_HOME, KC_END,
            KC_ESC,
            KC_RSHIFT, KC_ENT, KC_TAB),

        /* Keymap 3: BFV Layer
         *
         * ,-----------------------------------------------------.           ,-----------------------------------------------------.
         * |    ESC    | ---- | ---- | ---- | ---- | ---- | ---- |           | ---- | ---- | ---- | ---- | ---- | ---- |   ----    |
         * |           |      |      |      |      |      |      |           |      |      |      |      |      |      |           |
         * |-----------+------+------+------+------+------+------|           |------+------+------+------+------+------+-----------|
         * |    TAB    | ---- | ---- | ---- | ---- | ---- |   N  |           | ---- | ---- | ---- | ---- | ---- | ---- |   ----    |
         * |-----------+------+------+------+------+------|      |           |      |------+------+------+------+------+-----------|
         * |   LCTRL   | ---- | ---- | ---- | ---- | ---- |------|           |------| ---- | ---- | ---- | ---- | ---- |   ----    |
         * |-----------+------+------+------+------+------|      |           |      |------+------+------+------+------+-----------|
         * |     Z     | ---- | ---- | ---- | ---- | ---- |   M  |           | ---- | ---- | ---- | ---- | ---- | ---- |   ----    |
         * `-----------+------+------+------+------+-------------'           `-------------+------+------+------+------+-----------'
         *     | ----  | ---- | ---- | LALT |      |                                       | ---- | ---- | ---- | ---- | ---- |
         *     `-----------------------------------'                                       `----------------------------------'
         *                                         ,-------------.           ,-------------.
         *                                         | ---- | ---- |           | ---- | ---- |
         *                                  ,------|------|------|           |------+------+------.
         *                                  |      |      | ---- |           | ---- |      |      |
         *                                  | ---- | ---- |------|           |------| ---- | ---- |
         *                                  |      |      | ---- |           | ---- |      |      |
         *                                  `--------------------'           `--------------------'
         */
        [BFV] = LAYOUT_ergodox(
            // Left Hand
            KC_ESC, ____, ____, ____, ____, ____, ____,
            KC_TAB, KC_Q, KC_W, KC_E, KC_R, KC_T, KC_N,
            KC_LCTL, KC_A, KC_S, KC_D, KC_F, KC_G,
            KC_Z, KC_Z, KC_X, KC_C, KC_V, KC_B, KC_M,
            ____, ____, ____, KC_LALT, ____,
            ____, ____,
            ____,
            ____, ____, KC_LSHIFT,

            // Right Hand
            ____, ____, ____, ____, ____, ____, ____,
            ____, KC_Y, KC_U, KC_I, KC_O, KC_P, ____,
            KC_H, KC_J, KC_K, KC_L, KC_O, KC_SCOLON,
            ____, KC_N, KC_M, ____, ____, ____, ____,
            ____, ____, ____, ____, ____,
            ____, ____,
            ____,
            ____, ____, ____),

        /* Keymap 4: FORT Layer
         *
         * ,-----------------------------------------------------.           ,-----------------------------------------------------.
         * |    ----   | ---- | ---- | ---- | ---- | ---- | ---- |           | ---- | ---- | ---- | ---- | ---- | ---- |   ----    |
         * |           |      |      |      |      |      |      |           |      |      |      |      |      |      |           |
         * |-----------+------+------+------+------+------+------|           |------+------+------+------+------+------+-----------|
         * |   ----    | ---- | ---- | ---- | ---- | ---- | ---- |           | ---- | ---- | ---- | ---- | ---- | ---- |   ----    |
         * |-----------+------+------+------+------+------|      |           |      |------+------+------+------+------+-----------|
         * |   ----    | ---- | ---- | ---- | ---- | ---- |------|           |------| ---- | ---- | ---- | ---- | ---- |   ----    |
         * |-----------+------+------+------+------+------|      |           |      |------+------+------+------+------+-----------|
         * |   ----    | ---- | ---- | ---- | ---- | ---- | ---- |           | ---- | ---- | ---- | ---- | ---- | ---- |   ----    |
         * `-----------+------+------+------+------+-------------'           `-------------+------+------+------+------+-----------'
         *     | ----  | ---- | ---- | ---- | ---- |                                       | ---- | ---- | ---- | ---- | ---- |
         *     `-----------------------------------'                                       `----------------------------------'
         *                                         ,-------------.           ,-------------.
         *                                         | ---- | ---- |           | ---- | ---- |
         *                                  ,------|------|------|           |------+------+------.
         *                                  |      |      | ---- |           | ---- |      |      |
         *                                  | ---- | ---- |------|           |------| ---- | ---- |
         *                                  |      |      | ---- |           | ---- |      |      |
         *                                  `--------------------'           `--------------------'
         */
        [FORT] = LAYOUT_ergodox(
            // Left Hand
            ____, ____, ____, ____, ____, ____, ____,
            ____, ____, ____, ____, ____, ____, ____,
            ____, ____, ____, ____, ____, ____,
            ____, ____, ____, ____, ____, ____, ____,
            ____, ____, ____, ____, ____,
            ____, ____,
            ____,
            ____, ____, ____,

            // Right Hand
            ____, ____, ____, ____, ____, ____, ____,
            ____, ____, ____, ____, ____, ____, ____,
            ____, ____, ____, ____, ____, ____,
            ____, ____, ____, ____, ____, ____, ____,
            ____, ____, ____, ____, ____,
            ____, ____,
            ____,
            ____, ____, ____),
            
        /* Keymap 5: NUM Layer
         *
         * ,-----------------------------------------------------.           ,-----------------------------------------------------.
         * |    F1     |  F2  |  F3  |  F4  |  F5  |  F6  |      |           |      |   (  |   )  |   ^  |   /  | ---- |           |
         * |           |      |      |      |      |      | ---- |           | ---- |      |      |      |      |      |   ----    |
         * |-----------+------+------+------+------+------+------|           |------+------+------+------+------+------+-----------|
         * |    F7     |  F8  |  F9  | F10  | F11  | F12  |      |           |      |   7  |   8  |   9  |   *  | ---- |   ----    |
         * |-----------+------+------+------+------+------| ---- |           | ---- |------+------+------+------+------+-----------|
         * |   ----    | ---- | ---- | ---- | ---- | ---- |------|           |------|   4  |   5  |   6  |   -  | ---- |   ----    |
         * |-----------+------+------+------+------+------|      |           |      |------+------+------+------+------+-----------|
         * |   ----    | ---- | ---- | ---- | ---- | ---- | ---- |           | ---- |   1  |   2  |   3  |   +  | ---- |   ----    |
         * `-----------+------+------+------+------+-------------'           `-------------+------+------+------+------+-----------'
         *     |  ---- | ---- | ---- | ---- | ---- |                                       |   0  |   =  | ---- | ---- | ---- |
         *     `-----------------------------------'                                       `----------------------------------'
         *                                         ,-------------.           ,-------------.
         *                                         | ---- | ---- |           | ---- | ---- |
         *                                  ,------|------|------|           |------+------+------.
         *                                  |      |      | ---- |           | ---- |      |      |
         *                                  | ---- | ---- |------|           |------| ---- | ---- |
         *                                  |      |      | ---- |           | ---- |      |      |
         *                                  `--------------------'           `--------------------'
         */
        [NUM] = LAYOUT_ergodox(
            // Left Hand
            KC_F1, KC_F2, KC_F3, KC_F4, KC_F5, KC_F6, ____,
            KC_F7, KC_F8, KC_F9, KC_F10, KC_F11, KC_F12, ____,
            ____, ____, ____, ____, ____, ____,
            ____, ____, ____, ____, ____, ____, ____,
            ____, ____, ____, ____, ____,
            ____, ____,
            ____,
            ____, ____, ____,

            // Right Hand
            ____, KC_LPRN, KC_RPRN, KC_CIRC, KC_PSLS, ____, ____,
            ____, KC_7, KC_8, KC_9, KC_PAST, ____, ____,
            KC_4, KC_5, KC_6, KC_PMNS, ____, ____,
            ____, KC_1, KC_2, KC_3, KC_PPLS, ____, ____,
            KC_0, KC_PEQL, ____, ____, ____,
            ____, ____,
            ____,
            ____, ____, ____),

        /* Keymap 7: Configuration Layer
         *
         * ,-----------------------------------------------------.           ,-----------------------------------------------------.
         * |   ----    | ---- | ---- | ---- | ---- | ---- | ---- |           | PWR  | ---- | ---- | ---- | ---- | ---- |   ----    |
         * |           |      |      |      |      |      |      |           |      |      |      |      |      |      |           |
         * |-----------+------+------+------+------+------+------|           |------+------+------+------+------+------+-----------|
         * |   ----    | ---- | ---- | ---- | ---- | ---- |      |           |      | ---- | ---- | ---- | ---- | ---- |   ----    |
         * |-----------+------+------+------+------+------| ---- |           | SLP  |------+------+------+------+------+-----------|
         * |   ----    | ---- | ---- | ---- | ---- | ---- |------|           |------| ---- | ---- | ---- | ---- | ---- |   ----    |
         * |-----------+------+------+------+------+------|      |           |      |------+------+------+------+------+-----------|
         * |   ----    | ---- | ---- | ---- | ---- | ---- | ---- |           | WAKE | ---- | ---- | ---- | ---- | ---- |   ----    |
         * `-----------+------+------+------+------+-------------'           `-------------+------+------+------+------+-----------'
         *     | ----  | ---- | ---- | ---- | ---- |                                       | ---- | ---- | ---- | ---- | ----  |
         *     `-----------------------------------'                                       `-----------------------------------'
         *                                         ,-------------.           ,-------------.
         *                                         | ---- | ---- |           | TOG  | ANI  |
         *                                  ,------|------|------|           |------+------+------.
         *                                  |      |      | ---- |           | VAI |      |      |
         *                                  | ---- | ---- |------|           |------| HUI  | HUID |
         *                                  |      |      | ---- |           | VAD |      |      |
         *                                  `--------------------'           `--------------------'
         */
        [AUX] = LAYOUT_ergodox(
            // Left Hand
            ____, ____, ____, ____, ____, ____, KC_PWR,
            ____, ____, ____, ____, ____, ____, KC_SLEP,
            ____, ____, ____, ____, ____, ____,
            ____, ____, ____, ____, ____, ____, KC_WAKE,
            ____, ____, ____, ____, ____,
            ____, ____,
            ____,
            ____, ____, ____,
            // Right Hand
            KC_PWR, ____, ____, ____, ____, ____, ____,
            KC_SLEP, ____, ____, ____, ____, ____, ____,
            ____, ____, ____, ____, ____, ____,
            KC_WAKE, ____, ____, ____, ____, ____, ____,
            ____, ____, ____, ____, ____,
            RGB_TOG, M(RGB_ANI),
            RGB_VAI,
            RGB_VAD, RGB_HUI, RGB_HUD),
};

qk_tap_dance_action_t tap_dance_actions[] = {
        [TD_UNDO] = ACTION_TAP_DANCE_DOUBLE(LCTL(KC_Z), LCTL(KC_Y)),
        [TD_COPY] = ACTION_TAP_DANCE_DOUBLE(LCTL(KC_C), LCTL(KC_X)),
        [TD_FIND] = ACTION_TAP_DANCE_DOUBLE(LCTL(KC_F), LCTL(KC_H))};

const macro_t *action_get_macro(keyrecord_t *record, uint8_t id, uint8_t opt)
{
  switch (id)
  {
  case RGB_ANI:
    if (record->event.pressed)
    {
      rgb_timer = timer_read();
    }
    else
    {
      if (timer_elapsed(rgb_timer) > 300)
      {
        rgblight_mode(1);
      }
      else
      {
        rgblight_step();
      }
    }
  }
  return MACRO_NONE;
};

/*
bool process_record_user(uint16_t keycode, keyrecord_t *record)
{}
*/

void tap(uint16_t code)
{
  register_code(code);
  unregister_code(code);
}

void lin_umlaute(uint16_t code, uint8_t shift)
{
  tap(KC_RALT);
  register_code(KC_RSFT);
  tap(KC_QUOTE);
  if(shift == UPPER) {
    tap(code);
    unregister_code(KC_RSFT);
  } else {
    unregister_code(KC_RSFT);
    tap(code);
  }
}

void lin_accent(uint16_t code, uint8_t shift)
{
  tap(KC_RALT);
  tap(KC_QUOTE);
  if(shift == UPPER) {
    register_code(KC_RSFT);
    tap(code);
    unregister_code(KC_RSFT);
  } else {
    tap(code);
  }
}

void lin_tilde(uint16_t code, uint8_t shift)
{
  tap(KC_RALT);
  register_code(KC_RSFT);
  tap(KC_GRAVE);
  if(shift == UPPER) {
    tap(code);
    unregister_code(KC_RSFT);
  } else {
    unregister_code(KC_RSFT);
    tap(code);
  }
}

void lin_invert(uint16_t code)
{
  tap(KC_RALT);
  register_code(KC_RSFT);
  tap(code);
  tap(code);
  unregister_code(KC_RSFT);
}

void win_alt(const uint16_t code[])
{
  tap(KC_NLCK);
  register_code(KC_RALT);
  for(int i = 0; i < 4; i++)
  {
    tap(code[i]);
  }
  unregister_code(KC_RALT);
  tap(KC_NLCK);
}

LEADER_EXTERNS();
void matrix_scan_user(void)
{

  uint8_t layer = biton32(layer_state);

  if (keyboard_report->mods & MOD_BIT(KC_LSFT) ||
      ((get_oneshot_mods() & MOD_BIT(KC_LSFT)) &&
       !has_oneshot_mods_timed_out()))
  {
    ergodox_right_led_1_set(LED_BRIGHTNESS_HI);
    ergodox_right_led_1_on();
    ergodox_right_led_2_off();
    ergodox_right_led_3_off();
  }
  else if (keyboard_report->mods & MOD_BIT(KC_LCTL) ||
      ((get_oneshot_mods() & MOD_BIT(KC_LCTL)) &&
       !has_oneshot_mods_timed_out()))
  {
    ergodox_right_led_2_set(LED_BRIGHTNESS_HI);
    ergodox_right_led_2_on();
    ergodox_right_led_1_off();
    ergodox_right_led_3_off();
  }
  else if (keyboard_report->mods & MOD_BIT(KC_LALT) ||
      ((get_oneshot_mods() & MOD_BIT(KC_LALT)) &&
       !has_oneshot_mods_timed_out()))
  {
    ergodox_right_led_3_set(LED_BRIGHTNESS_HI);
    ergodox_right_led_3_on();
    ergodox_right_led_1_off();
    ergodox_right_led_2_off();
  }
  else
  {
    switch(layer)
    {
      case COLE:    ergodox_right_led_3_set(LED_BRIGHTNESS_LO);
                    ergodox_right_led_3_on();
                    ergodox_right_led_1_off();
                    ergodox_right_led_2_off();
                    break;
      case QWERTY:  ergodox_right_led_1_set(LED_BRIGHTNESS_LO);
                    ergodox_right_led_1_on();
                    ergodox_right_led_2_off();
                    ergodox_right_led_3_off();
                    break;
      case NUM:     ergodox_right_led_2_set(LED_BRIGHTNESS_LO);
                    ergodox_right_led_2_on();
                    ergodox_right_led_1_off();
                    ergodox_right_led_3_off();
                    break;
      case BFV:     if (prev_layer != layer)
                    {
                      layer_timer = timer_read();
                      ergodox_right_led_1_set(LED_BRIGHTNESS_HI);
                      ergodox_right_led_1_on();
                      led_on = true;
                    }
                    else if (timer_elapsed(layer_timer) > 1000)
                    {
                      if (led_on)
                      {
                        ergodox_right_led_1_off();
                      }
                      else
                      {
                        ergodox_right_led_1_on();
                      }
                      layer_timer = timer_read();
                      led_on = !led_on;
                    }
                    ergodox_right_led_2_off();
                    ergodox_right_led_3_off();
                    break;
      case FORT:    if (prev_layer != layer)
                    {
                      layer_timer = timer_read();
                      ergodox_right_led_3_set(LED_BRIGHTNESS_HI);
                      ergodox_right_led_3_on();
                      led_on = true;
                    }
                    else if (timer_elapsed(layer_timer) > 1000)
                    {
                      if (led_on)
                      {
                        ergodox_right_led_3_off();
                      }
                      else
                      {
                        ergodox_right_led_3_on();
                      }
                      layer_timer = timer_read();
                      led_on = !led_on;
                    }
                    ergodox_right_led_1_off();
                    ergodox_right_led_2_off();
                    break;
      case AUX:     ergodox_right_led_1_set(LED_BRIGHTNESS_LO);
                    ergodox_right_led_2_set(LED_BRIGHTNESS_LO);
                    ergodox_right_led_3_set(LED_BRIGHTNESS_LO);
                    ergodox_right_led_1_on();
                    ergodox_right_led_2_on();
                    ergodox_right_led_3_on();
                    break;
    }
  }

  prev_layer = layer;

  LEADER_DICTIONARY()
  {
    leading = false;
    leader_end();

    SEQ_THREE_KEYS(KC_W, KC_I, KC_N) { os_type = OS_WIN; };
    SEQ_THREE_KEYS(KC_O, KC_S, KC_X) { os_type = OS_OSX; };
    SEQ_THREE_KEYS(KC_L, KC_I, KC_N) { os_type = OS_LIN; };

    SEQ_TWO_KEYS(KC_D, KC_E) { lang = DE; };
    SEQ_TWO_KEYS(KC_E, KC_S) { lang = ES; };

    SEQ_ONE_KEY(KC_QUES)
    {
      switch(lang)
      {
        case ES:
          switch (os_type)
          {
          case OS_WIN:
            win_alt((const uint16_t[]){KC_KP_0, KC_KP_1, KC_KP_9, KC_KP_1});
            break;
          case OS_OSX:
            break;
          case OS_LIN:
            lin_invert(KC_SLSH);
            break;
          }
          break;
      }
    }
    SEQ_ONE_KEY(KC_EXLM)
    {
      switch(lang)
      {
        case ES:
          switch (os_type)
          {
          case OS_WIN:
            win_alt((const uint16_t[]){KC_KP_0, KC_KP_1, KC_KP_6, KC_KP_1});
            break;
          case OS_OSX:
            break;
          case OS_LIN:
            lin_invert(KC_1);
            break;
          }
          break;
      }
    }
    SEQ_ONE_KEY(KC_A)
    {
      switch (os_type)
      {
      case OS_WIN:
        switch (lang)
        {
          case DE:
            win_alt((const uint16_t[]){KC_KP_0, KC_KP_2, KC_KP_2, KC_KP_8});
            break;
          case ES:
            win_alt((const uint16_t[]){KC_KP_0, KC_KP_2, KC_KP_2, KC_KP_5});
            break;
        }
        break;
      case OS_OSX:
        register_code(KC_RALT);
        register_code(KC_RSFT);
        register_code(KC_SCLN);
        unregister_code(KC_SCLN);
        unregister_code(KC_RSFT);
        unregister_code(KC_RALT);
        tap(KC_A);
        break;
      case OS_LIN:
        switch (lang)
        {
          case DE:
            lin_umlaute(KC_A, LOWER);
            break;
          case ES:
            lin_accent(KC_A, LOWER);
            break;
        }
        break;
      }
    }
    SEQ_TWO_KEYS(KC_A, KC_A)
    {
      switch (os_type)
      {
      case OS_WIN:
        switch (lang)
        {
          case DE:
            win_alt((const uint16_t[]){KC_KP_0, KC_KP_1, KC_KP_9, KC_KP_6});
            break;
          case ES:
            win_alt((const uint16_t[]){KC_KP_0, KC_KP_1, KC_KP_9, KC_KP_3});
            break;
        }
        break;
      case OS_OSX:
        register_code(KC_RALT);
        register_code(KC_RSFT);
        register_code(KC_SCLN);
        unregister_code(KC_SCLN);
        unregister_code(KC_RSFT);
        unregister_code(KC_RALT);
        register_code(KC_LSFT);
        register_code(KC_A);
        unregister_code(KC_A);
        unregister_code(KC_LSFT);
        break;
      case OS_LIN:
        switch (lang)
        {
          case DE:
            lin_umlaute(KC_A, UPPER);
            break;
          case ES:
            lin_accent(KC_A, UPPER);
            break;
        }
        break;
      }
    }
    SEQ_ONE_KEY(KC_E)
    {
      switch(lang)
      {
        case ES:
          switch (os_type)
          {
          case OS_WIN:
            win_alt((const uint16_t[]){KC_KP_0, KC_KP_2, KC_KP_3, KC_KP_3});
            break;
          case OS_OSX:
            break;
          case OS_LIN:
            lin_accent(KC_E, LOWER);
            break;
          }
          break;
      }
    }
    SEQ_TWO_KEYS(KC_E, KC_E)
    {
      switch(lang)
      {
        case ES:
          switch (os_type)
          {
          case OS_WIN:
            win_alt((const uint16_t[]){KC_KP_0, KC_KP_2, KC_KP_0, KC_KP_1});
            break;
          case OS_OSX:
            break;
          case OS_LIN:
            lin_accent(KC_E, UPPER);
            break;
          }
          break;
      }
    }
    SEQ_ONE_KEY(KC_I)
    {
      switch(lang)
      {
        case ES:
          switch (os_type)
          {
          case OS_WIN:
            win_alt((const uint16_t[]){KC_KP_0, KC_KP_2, KC_KP_3, KC_KP_7});
            break;
          case OS_OSX:
            break;
          case OS_LIN:
            lin_accent(KC_I, LOWER);
            break;
          }
          break;
      }
    }
    SEQ_TWO_KEYS(KC_I, KC_I)
    {
      switch(lang)
      {
        case ES:
          switch (os_type)
          {
          case OS_WIN:
            win_alt((const uint16_t[]){KC_KP_0, KC_KP_2, KC_KP_0, KC_KP_5});
            break;
          case OS_OSX:
            break;
          case OS_LIN:
            lin_accent(KC_I, UPPER);
            break;
          }
          break;
      }
    }
    SEQ_ONE_KEY(KC_N)
    {
      switch(lang)
      {
        case ES:
          switch (os_type)
          {
          case OS_WIN:
            win_alt((const uint16_t[]){KC_KP_0, KC_KP_2, KC_KP_4, KC_KP_1});
            break;
          case OS_OSX:
            break;
          case OS_LIN:
            lin_tilde(KC_N, LOWER);
            break;
          }
          break;
      }
    }
    SEQ_TWO_KEYS(KC_N, KC_N)
    {
      switch(lang)
      {
        case ES:
          switch (os_type)
          {
          case OS_WIN:
            win_alt((const uint16_t[]){KC_KP_0, KC_KP_2, KC_KP_0, KC_KP_9});
            break;
          case OS_OSX:
            break;
          case OS_LIN:
            lin_tilde(KC_N, UPPER);
            break;
          }
          break;
      }
    }
    SEQ_ONE_KEY(KC_O)
    {
      switch (os_type)
      {
      case OS_WIN:
        switch (lang)
        {
          case DE:
            win_alt((const uint16_t[]){KC_KP_0, KC_KP_2, KC_KP_4, KC_KP_6});
            break;
          case ES:
            win_alt((const uint16_t[]){KC_KP_0, KC_KP_2, KC_KP_4, KC_KP_3});
            break;
        }
        break;
      case OS_OSX:
        register_code(KC_RALT);
        register_code(KC_RSFT);
        register_code(KC_SCLN);
        unregister_code(KC_SCLN);
        unregister_code(KC_RSFT);
        unregister_code(KC_RALT);
        tap(KC_O);
        break;
      case OS_LIN:
        switch (lang)
        {
          case DE:
            lin_umlaute(KC_O, LOWER);
            break;
          case ES:
            lin_accent(KC_O, LOWER);
            break;
        }
        break;
      }
    }
    SEQ_TWO_KEYS(KC_O, KC_O)
    {
      switch (os_type)
      {
      case OS_WIN:
        switch (lang)
        {
          case DE:
            win_alt((const uint16_t[]){KC_KP_0, KC_KP_2, KC_KP_1, KC_KP_4});
            break;
          case ES:
            win_alt((const uint16_t[]){KC_KP_0, KC_KP_2, KC_KP_1, KC_KP_1});
            break;
        }
        break;
      case OS_OSX:
        register_code(KC_RALT);
        register_code(KC_RSFT);
        register_code(KC_SCLN);
        unregister_code(KC_SCLN);
        unregister_code(KC_RSFT);
        unregister_code(KC_RALT);
        tap(LSFT(KC_O));
        break;
      case OS_LIN:
        switch (lang)
        {
          case DE:
            lin_umlaute(KC_O, UPPER);
            break;
          case ES:
            lin_accent(KC_O, UPPER);
            break;
        }
        break;
      }
    }
    SEQ_ONE_KEY(KC_U)
    {
      switch (os_type)
      {
      case OS_WIN:
        switch (lang)
        {
          case DE:
            win_alt((const uint16_t[]){KC_KP_0, KC_KP_2, KC_KP_5, KC_KP_2});
            break;
          case ES:
            win_alt((const uint16_t[]){KC_KP_0, KC_KP_2, KC_KP_5, KC_KP_0});
            break;
        }
        break;
      case OS_OSX:
        register_code(KC_RALT);
        register_code(KC_RSFT);
        register_code(KC_SCLN);
        unregister_code(KC_SCLN);
        unregister_code(KC_RSFT);
        unregister_code(KC_RALT);
        tap(KC_U);
        break;
      case OS_LIN:
        switch (lang)
        {
          case DE:
            lin_umlaute(KC_U, LOWER);
            break;
          case ES:
            lin_accent(KC_U, LOWER);
            break;
        }
        break;
      }
    }
    SEQ_TWO_KEYS(KC_U, KC_U)
    {
      switch (os_type)
      {
      case OS_WIN:
        switch (lang)
        {
          case DE:
            win_alt((const uint16_t[]){KC_KP_0, KC_KP_2, KC_KP_2, KC_KP_0});
            break;
          case ES:
            win_alt((const uint16_t[]){KC_KP_0, KC_KP_2, KC_KP_1, KC_KP_8});
            break;
        }
        break;
      case OS_OSX:
        register_code(KC_RALT);
        register_code(KC_RSFT);
        register_code(KC_SCLN);
        unregister_code(KC_SCLN);
        unregister_code(KC_RSFT);
        unregister_code(KC_RALT);
        tap(LSFT(KC_U));
        break;
      case OS_LIN:
        switch (lang)
        {
          case DE:
            lin_umlaute(KC_U, UPPER);
            break;
          case ES:
            lin_accent(KC_U, UPPER);
            break;
        }
        break;
      }
    }
    SEQ_THREE_KEYS(KC_U, KC_U, KC_U)
    {
      switch (lang)
      {
        case ES:
          switch (os_type)
          {
            case OS_WIN:
              win_alt((const uint16_t[]){KC_KP_0, KC_KP_2, KC_KP_2, KC_KP_0});
              break;
            case OS_OSX:
              register_code(KC_RALT);
              register_code(KC_RSFT);
              register_code(KC_SCLN);
              unregister_code(KC_SCLN);
              unregister_code(KC_RSFT);
              unregister_code(KC_RALT);
              tap(LSFT(KC_U));
              break;
            case OS_LIN:
              lin_umlaute(KC_U, UPPER);
              break;
          }
          break;
      }
    }
    SEQ_THREE_KEYS(KC_U, KC_U, KC_U)
    {
      switch (lang)
      {
        case ES:
          switch (os_type)
          {
            case OS_WIN:
              win_alt((const uint16_t[]){KC_KP_0, KC_KP_2, KC_KP_2, KC_KP_0});
              break;
            case OS_OSX:
              register_code(KC_RALT);
              register_code(KC_RSFT);
              register_code(KC_SCLN);
              unregister_code(KC_SCLN);
              unregister_code(KC_RSFT);
              unregister_code(KC_RALT);
              tap(LSFT(KC_U));
              break;
            case OS_LIN:
              lin_umlaute(KC_U, UPPER);
              break;
          }
          break;
      }
    }
    SEQ_ONE_KEY(KC_S)
    {
      switch (lang)
      {
        case DE:
          switch (os_type)
          {
          case OS_WIN:
            win_alt((const uint16_t[]){KC_KP_0, KC_KP_2, KC_KP_2, KC_KP_3});
            break;
          case OS_OSX:
            register_code(KC_RALT);
            tap(KC_S);
            unregister_code(KC_RALT);
            break;
          case OS_LIN:
            tap(KC_RALT);
            tap(KC_S);
            tap(KC_S);
            break;
          }
        break;
      }
    }
  }
}

void matrix_init_user(void)
{

  ergodox_led_all_on();
  rgblight_init();
  rgblight_enable();
  rgblight_setrgb(255, 0, 0);
  for (int i = LED_BRIGHTNESS_HI; i > LED_BRIGHTNESS_LO; i--)
  {
    ergodox_led_all_set(i);
    wait_ms(5);
  }
  rgblight_setrgb(255, 255, 0);
  wait_ms(1000);
  for (int i = LED_BRIGHTNESS_LO; i > 0; i--)
  {
    ergodox_led_all_set(i);
    wait_ms(10);
  }
  rgblight_setrgb(0, 255, 255);
  ergodox_led_all_off();
  wait_ms(1000);

  rgblight_mode(RGBLIGHT_MODE_KNIGHT + 1);
}