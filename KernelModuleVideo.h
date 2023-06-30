#ifndef _KERNEL_MODULE_VIDEO_H_
#define _KERNEL_MODULE_VIDEO_H_

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fb.h>
#include <linux/fs.h>
#include <linux/slab.h> 



int video_module_init(void);
void video_module_exit(void);

#endif