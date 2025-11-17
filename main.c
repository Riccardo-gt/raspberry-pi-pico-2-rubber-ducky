/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Ha Thach (tinyusb.org)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "bsp/board_api.h"
#include "tusb.h"

#include "usb_descriptors.h"


//--------------------------------------------------------------------+
// ASCII TO HID
//--------------------------------------------------------------------+

// ---------- Modifiers ----------
#define MOD_CTRL  KEYBOARD_MODIFIER_LEFTCTRL
#define MOD_SHIFT KEYBOARD_MODIFIER_LEFTSHIFT
#define MOD_ALT   KEYBOARD_MODIFIER_LEFTALT
#define MOD_GUI   KEYBOARD_MODIFIER_LEFTGUI

// ---------- ASCII → HID mapping ----------

static const uint8_t hid_ascii_usage[128] = {
    // letters
    ['a'] = HID_KEY_A, ['A'] = HID_KEY_A,
    ['b'] = HID_KEY_B, ['B'] = HID_KEY_B,
    ['c'] = HID_KEY_C, ['C'] = HID_KEY_C,
    ['d'] = HID_KEY_D, ['D'] = HID_KEY_D,
    ['e'] = HID_KEY_E, ['E'] = HID_KEY_E,
    ['f'] = HID_KEY_F, ['F'] = HID_KEY_F,
    ['g'] = HID_KEY_G, ['G'] = HID_KEY_G,
    ['h'] = HID_KEY_H, ['H'] = HID_KEY_H,
    ['i'] = HID_KEY_I, ['I'] = HID_KEY_I,
    ['j'] = HID_KEY_J, ['J'] = HID_KEY_J,
    ['k'] = HID_KEY_K, ['K'] = HID_KEY_K,
    ['l'] = HID_KEY_L, ['L'] = HID_KEY_L,
    ['m'] = HID_KEY_M, ['M'] = HID_KEY_M,
    ['n'] = HID_KEY_N, ['N'] = HID_KEY_N,
    ['o'] = HID_KEY_O, ['O'] = HID_KEY_O,
    ['p'] = HID_KEY_P, ['P'] = HID_KEY_P,
    ['q'] = HID_KEY_Q, ['Q'] = HID_KEY_Q,
    ['r'] = HID_KEY_R, ['R'] = HID_KEY_R,
    ['s'] = HID_KEY_S, ['S'] = HID_KEY_S,
    ['t'] = HID_KEY_T, ['T'] = HID_KEY_T,
    ['u'] = HID_KEY_U, ['U'] = HID_KEY_U,
    ['v'] = HID_KEY_V, ['V'] = HID_KEY_V,
    ['w'] = HID_KEY_W, ['W'] = HID_KEY_W,
    ['x'] = HID_KEY_X, ['X'] = HID_KEY_X,
    ['y'] = HID_KEY_Y, ['Y'] = HID_KEY_Y,
    ['z'] = HID_KEY_Z, ['Z'] = HID_KEY_Z,

    // numbers & shifted symbols
    ['1'] = HID_KEY_1, ['!'] = HID_KEY_1,
    ['2'] = HID_KEY_2, ['@'] = HID_KEY_2,
    ['3'] = HID_KEY_3, ['#'] = HID_KEY_3,
    ['4'] = HID_KEY_4, ['$'] = HID_KEY_4,
    ['5'] = HID_KEY_5, ['%'] = HID_KEY_5,
    ['6'] = HID_KEY_6, ['^'] = HID_KEY_6,
    ['7'] = HID_KEY_7, ['&'] = HID_KEY_7,
    ['8'] = HID_KEY_8, ['*'] = HID_KEY_8,
    ['9'] = HID_KEY_9, ['('] = HID_KEY_9,
    ['0'] = HID_KEY_0, [')'] = HID_KEY_0,

    // whitespace
    [' ']  = HID_KEY_SPACE,
    ['\n'] = HID_KEY_ENTER,
    ['\r'] = HID_KEY_ENTER,
    ['\t'] = HID_KEY_TAB,

    // punctuation
    ['-'] = HID_KEY_MINUS, ['_'] = HID_KEY_MINUS,
    ['='] = HID_KEY_EQUAL, ['+'] = HID_KEY_EQUAL,

    ['['] = HID_KEY_BRACKET_LEFT,  ['{'] = HID_KEY_BRACKET_LEFT,
    [']'] = HID_KEY_BRACKET_RIGHT, ['}'] = HID_KEY_BRACKET_RIGHT,

    ['\\'] = HID_KEY_BACKSLASH, ['|'] = HID_KEY_BACKSLASH,

    [';'] = HID_KEY_SEMICOLON, [':'] = HID_KEY_SEMICOLON,

    ['\''] = HID_KEY_APOSTROPHE, ['"'] = HID_KEY_APOSTROPHE,

    ['`'] = HID_KEY_GRAVE, ['~'] = HID_KEY_GRAVE,

    [','] = HID_KEY_COMMA,  ['<'] = HID_KEY_COMMA,
    ['.'] = HID_KEY_PERIOD, ['>'] = HID_KEY_PERIOD,
    ['/'] = HID_KEY_SLASH,  ['?'] = HID_KEY_SLASH,
};

static const uint8_t hid_ascii_shift[128] = {
    // uppercase
    ['A'] = MOD_SHIFT, ['B'] = MOD_SHIFT, ['C'] = MOD_SHIFT, ['D'] = MOD_SHIFT,
    ['E'] = MOD_SHIFT, ['F'] = MOD_SHIFT, ['G'] = MOD_SHIFT, ['H'] = MOD_SHIFT,
    ['I'] = MOD_SHIFT, ['J'] = MOD_SHIFT, ['K'] = MOD_SHIFT, ['L'] = MOD_SHIFT,
    ['M'] = MOD_SHIFT, ['N'] = MOD_SHIFT, ['O'] = MOD_SHIFT, ['P'] = MOD_SHIFT,
    ['Q'] = MOD_SHIFT, ['R'] = MOD_SHIFT, ['S'] = MOD_SHIFT, ['T'] = MOD_SHIFT,
    ['U'] = MOD_SHIFT, ['V'] = MOD_SHIFT, ['W'] = MOD_SHIFT, ['X'] = MOD_SHIFT,
    ['Y'] = MOD_SHIFT, ['Z'] = MOD_SHIFT,

    // shifted symbols
    ['!'] = MOD_SHIFT, ['@'] = MOD_SHIFT, ['#'] = MOD_SHIFT,
    ['$'] = MOD_SHIFT, ['%'] = MOD_SHIFT, ['^'] = MOD_SHIFT,
    ['&'] = MOD_SHIFT, ['*'] = MOD_SHIFT, ['('] = MOD_SHIFT,
    [')'] = MOD_SHIFT, ['_'] = MOD_SHIFT, ['+'] = MOD_SHIFT,
    ['{'] = MOD_SHIFT, ['}'] = MOD_SHIFT, ['|'] = MOD_SHIFT,
    [':'] = MOD_SHIFT, ['"'] = MOD_SHIFT, ['~'] = MOD_SHIFT,
    ['<'] = MOD_SHIFT, ['>'] = MOD_SHIFT, ['?'] = MOD_SHIFT,
};


//--------------------------------------------------------------------+
// MACRO CONSTANT TYPEDEF PROTYPES
//--------------------------------------------------------------------+

/* Blink pattern
 * - 250 ms  : device not mounted
 * - 1000 ms : device mounted
 * - 2500 ms : device is suspended
 */
enum  {
  BLINK_NOT_MOUNTED = 250,
  BLINK_MOUNTED = 1000,
  BLINK_SUSPENDED = 2500,
};

static uint32_t blink_interval_ms = BLINK_NOT_MOUNTED;


// Typing speed, lower the more buggy
#define WAIT_STRING 3
#define KEY_RELEASE 5


// ---------- Typing helpers ----------

static inline bool send_hid_report(uint8_t report_id, char c, uint8_t modifiers);

bool type_char(char c)
{
  /*
  // Poll every 10ms
  const uint32_t interval_ms = 10;
  static uint32_t start_ms = 0;

  if ( board_millis() - start_ms < interval_ms) return false; // not enough time
  start_ms += interval_ms;
*/
  // Remote wakeup
  if ( tud_suspended())
  {
    // Wake up host if we are in suspend mode
    // and REMOTE_WAKEUP feature is enabled by host
    tud_remote_wakeup();
	return false;
  }else
  {
    bool works = send_hid_report(REPORT_ID_KEYBOARD, c, 0);
  	sleep_ms(KEY_RELEASE);
  	// key up (release all keys)
	tud_hid_keyboard_report(REPORT_ID_KEYBOARD, 0, NULL);
  	sleep_ms(KEY_RELEASE);
	return works;
  }
}

static bool type_char_with_mods(char c, uint8_t extra_mods) {
  if ( tud_suspended())
  {
    tud_remote_wakeup();
	return false;
  } else
  {
    bool works = send_hid_report(REPORT_ID_KEYBOARD, c, extra_mods);
  	sleep_ms(KEY_RELEASE);
  	// key up (release all keys)
	tud_hid_keyboard_report(REPORT_ID_KEYBOARD, 0, NULL);
  	sleep_ms(KEY_RELEASE);
	return works;
   }
}

static inline void type_string(const char *s) {
    while (*s) {
		tud_task(); // tinyusb device task
  		sleep_ms(WAIT_STRING);
      	if (type_char(*s)) s++;
    }
}

static inline void type_string_with_mods(const char *s, uint8_t extra_mods) {
    while (*s) {
		tud_task(); // tinyusb device task
  		sleep_ms(WAIT_STRING);
      	if (type_char_with_mods(*s, extra_mods)) s++;
    }
}



// ---------- Commonly used ----------

static inline void press_win_r(void) {
    type_char_with_mods('r', MOD_GUI);
}

static inline void press_enter(void) {
    type_char('\n');
}

static void cmd(void) {
    press_win_r();

    // 2) Give Windows time to show the Run dialog
    sleep_ms(600);

    type_string("cmd");

    type_char_with_mods('\n', MOD_CTRL | MOD_SHIFT);

}

/*------------- MAIN -------------*/
int main(void)
{
	board_init();
	// init device stack on configured roothub port
	tud_init(BOARD_TUD_RHPORT);

	while (!tud_mounted()) {
		tud_task();
		sleep_ms(10);
	}
	sleep_ms(2000);

	if (board_init_after_tusb) {
		board_init_after_tusb();
	}

	while (!board_button_read()) {
		sleep_ms(50);
	}

	type_string("hello");
	type_string_with_mods("hello", MOD_SHIFT);

}



//--------------------------------------------------------------------+
//--------------------------------------------------------------------+

// Invoked when device is mounted
void tud_mount_cb(void)
{
  blink_interval_ms = BLINK_MOUNTED;
}

// Invoked when device is unmounted
void tud_umount_cb(void)
{
  blink_interval_ms = BLINK_NOT_MOUNTED;
}

// Invoked when usb bus is suspended
// remote_wakeup_en : if host allow us  to perform remote wakeup
// Within 7ms, device must draw an average of current less than 2.5 mA from bus
void tud_suspend_cb(bool remote_wakeup_en)
{
  (void) remote_wakeup_en;
  blink_interval_ms = BLINK_SUSPENDED;
}

// Invoked when usb bus is resumed
void tud_resume_cb(void)
{
  blink_interval_ms = tud_mounted() ? BLINK_MOUNTED : BLINK_NOT_MOUNTED;
}





//--------------------------------------------------------------------+
// USB HID
//--------------------------------------------------------------------+

static inline bool send_hid_report(uint8_t report_id, char c, uint8_t modifiers)
{
  // skip if hid is not ready yet
  if ( !tud_hid_ready() ) return false;

	uint8_t keycode[6] = { 0 };
	keycode[0] = hid_ascii_usage[c];

	return tud_hid_keyboard_report(REPORT_ID_KEYBOARD, modifiers, keycode);
}


// Invoked when sent REPORT successfully to host
// Application can use this to send the next report
// Note: For composite reports, report[0] is report ID
void tud_hid_report_complete_cb(uint8_t instance, uint8_t const* report, uint16_t len)
{
  (void) instance;
  (void) len;
  return;
}

// Invoked when received GET_REPORT control request
// Application must fill buffer report's content and return its length.
// Return zero will cause the stack to STALL request
uint16_t tud_hid_get_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t* buffer, uint16_t reqlen)
{
  // TODO not Implemented
  (void) instance;
  (void) report_id;
  (void) report_type;
  (void) buffer;
  (void) reqlen;

  return 0;
}

// Invoked when received SET_REPORT control request or
// received data on OUT endpoint ( Report ID = 0, Type = 0 )
void tud_hid_set_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t const* buffer, uint16_t bufsize)
{
  (void) instance;

  if (report_type == HID_REPORT_TYPE_OUTPUT)
  {
    // Set keyboard LED e.g Capslock, Numlock etc...
    if (report_id == REPORT_ID_KEYBOARD)
    {
      // bufsize should be (at least) 1
      if ( bufsize < 1 ) return;

      uint8_t const kbd_leds = buffer[0];

      if (kbd_leds & KEYBOARD_LED_CAPSLOCK)
      {
        // Capslock On: disable blink, turn led on
        blink_interval_ms = 0;
        board_led_write(true);
      }else
      {
        // Caplocks Off: back to normal blink
        board_led_write(false);
        blink_interval_ms = BLINK_MOUNTED;
      }
    }
  }
}


