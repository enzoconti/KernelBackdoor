
#ifndef INIT_H
#define INIT_H

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>

#endif

#ifndef KEYBOARD_MODULE_H
#define KEYBOARD_MODULE_H

char *get_key_buffer(size_t *position);

static int __init keyboard_module_init(void);

static void __exit keyboard_module_exit(void);

#endif /* KEYBOARD_MODULE_H */