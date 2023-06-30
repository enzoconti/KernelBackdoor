# KernelBackdoor

An educationally-purposed backdoor of linux kernel that sweeps keyboard data to another machine via network

We implemented two kernel modules. The first one is responsible for logging keyboard data and saving to a file on the debug file space. The second creates a network connection and sends that file to another machine.

The working code is on the singleFile-Working folder.
To compile the module go in that folder and run: make all.

To insert the module run: make insert

To see the buffer received via network run: python3 python_client.py

To see the keyboard log locally run: sudo cat /sys/kernel/debug/backdoorSO/bdKeyboardLog

To remove the module run: make remove

Alunos:
Nicholas Estevao Pereira de Oliveira Rodrigues Braganca - 12689616
Enzo Conti - 12547145
Rafael Freitas Garcia - 11222374
