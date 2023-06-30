
#ifndef INIT_H
#define INIT_H

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>

#endif

#ifndef SERVER_H
#define SERVER_H

static int __init server_module_init(void);
static void __exit server_module_exit(void);

#endif