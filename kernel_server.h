#ifndef __KERNEL_SERVER_H__
#define __KERNEL_SERVER_H__

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/net.h>
#include <linux/in.h>
#include <linux/socket.h>
#include <linux/kthread.h>
#include <linux/net.h>
#include <linux/nsproxy.h>

#include "keyboardmodule.h"


int listen_thread_func(void *data);
int server_module_init(void);
void server_module_exit(void);

#endif