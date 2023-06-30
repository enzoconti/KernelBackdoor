KDIR ?= /lib/modules/$(shell uname -r)/build

.PHONY: all clean module insert remove

all: module

insert:
	sudo insmod src/backdoor.ko

remove:
	sudo rmmod src/backdoor.ko

clean:
	$(MAKE) -C $(KDIR) M=$(PWD)/src clean

module:
	make -C  /lib/modules/$(shell uname -r)/build M=$(PWD)/src modules