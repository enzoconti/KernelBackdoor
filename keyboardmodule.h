#ifndef KEYBOARDMODULE_H
#define KEYBOARDMODULE_H

#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/keyboard.h>
#include <linux/debugfs.h>
#include <linux/input.h>

int keyboard_callback(struct notifier_block *nblock, long unsigned int code, void *_param);
int keyboard_module_init(void);
void keyboard_module_exit(void);
void keycode_to_string(int keycode, int shift_mask, char *buf);

#endif