#ifndef _KERNEL_MODULE_VIDEO_H_
#define _KERNEL_MODULE_VIDEO_H_

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fb.h>
#include <linux/fs.h>
#include <linux/slab.h> 
#include <sys/mman.h> 

u8 *pixel_data_buffer ; // pointer to store the RGB data
int xres, yres; // loop counter and return value for functions that return an integer value (error checking) 

static int __init video_module_init(void);
static void __exit video_module_exit(void);

#endif