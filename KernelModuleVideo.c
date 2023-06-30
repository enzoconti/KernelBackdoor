#include "KernelModuleVideo.h"

static struct fb_info *fb_info = NULL; // framebuffer information structure pointer that contains the screen information (resolution, pixel format, etc.)

/**
 * @brief Module initialization function
 * @return 0 on success, error code otherwise
 * @note This function is called when the module is loaded
 * @note This function is static because it should not be visible outside this file
*/
static int __init video_module_init(void)
{
    struct file *fb_file; // framebuffer device file pointer
    struct fb_info *fb_info; // framebuffer information structure pointer that contains the screen information (resolution, pixel format, etc.)
    void *buffer; // pointer to store the framebuffer data
    long fb_size; // variable to store the size of the framebuffer in bytes
    void *fb_start_pointer; // pointer to the start of the framebuffer

    u32 *pixel; // pointer to a pixel (32 bits) 
    u32 color; // variable to store the color of a pixel (32 bits)
    u8 r, g, b; // variables to store the RGB values of a pixel (8 bits per channel)
    

    printk(KERN_INFO "Video module initialized\n");

    // Open the framebuffer device file
    fb_file = filp_open("/dev/fb0", O_RDONLY, 0); // O_RDONLY = read only mode 
    if (IS_ERR(fb_file)) {// error checking (if the pointer is NULL it means that the framebuffer device file was not found)
        printk(KERN_ERR "Failed to open framebuffer device\n");
        return PTR_ERR(fb_file); 
    }

    fb_info = fb_file->private_data; // get the framebuffer information structure pointer from the framebuffer device file
    fb_size = fb_info->screen_size; // get the size of the framebuffer in bytes
    fb_start_pointer = fb_info->screen_base; // get the start address of the framebuffer
    yres = fb_info->var.yres; // get the vertical resolution of the screen
    xres = fb_info->var.xres; // get the horizontal resolution of the screen

    buffer = kmalloc(fb_size, GFP_KERNEL); // allocate memory for the framebuffer data, GFP_KERNEL = allocate normal kernel ram
    if (!buffer) { // error checking (if the pointer is NULL it means that the framebuffer device file was not mapped into memory)
        printk(KERN_ALERT "Memory allocation failed\n");
        return PTR_ERR(buffer); 
    }
    memcpy(buffer, fb_start_pointer, fb_size); // copy the framebuffer data to the buffer
    pixel_data_buffer  = kmalloc(yres * xres * 3, GFP_KERNEL); // allocate memory for the RGB data

    for (int y = 0; y < yres; y++) {
        for (int x = 0; x < xres; x++) {
            color = *pixel++;
            r = (color >> fb_info->var.red.offset) & ((1 << fb_info->var.red.length) - 1);
            g = (color >> fb_info->var.green.offset) & ((1 << fb_info->var.green.length) - 1);
            b = (color >> fb_info->var.blue.offset) & ((1 << fb_info->var.blue.length) - 1);

            // Calculate the index of the pixel in the buffer by multiplying the y coordinate by the horizontal resolution and adding the x coordinate because the pixels are stored in a linear array 
            int pixel_index = (y * xres + x) * 3;

            // Store the RGB values in the buffer
            pixel_data_buffer[pixel_index] = r;
            pixel_data_buffer[pixel_index + 1] = g;
            pixel_data_buffer[pixel_index + 2] = b;
        }
    }

    filp_close(fb_file, NULL); // close the framebuffer file
    kfree(buffer); // free the memory allocated for the framebuffer data
    return 0;
}


/**
 * This function is called when the module is unloaded.
 */
static void __exit video_module_exit(void)
{
    printk(KERN_INFO "Video module exited\n"); // print to the kernel log with the KERN_INFO priority level
}


module_init(video_module_init); // module initialization function
module_exit(video_module_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("NicholasEnzoRafael");
MODULE_DESCRIPTION("Video Module");