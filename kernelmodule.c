#include "kernel_server.h"
#include "keyboardmodule.h"

int __init kernel_module_init(void)
{
	keyboard_module_init();
	server_module_init();
	return 0;
}

void __exit kernel_module_exit(void)
{
	server_module_exit();
	keyboard_module_exit();
}

module_init(kernel_module_init);
module_exit(kernel_module_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("NicholasEnzoRafael");
MODULE_DESCRIPTION("Kernel Module");