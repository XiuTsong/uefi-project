# uefi-project1
	UEFI project for SJTU software engineering graduate course.

## Prerequesites

1. Update the package managers database.

```bash
sudo apt-get update
```

2. Install the following packages.

```bash
sudo apt-get install  \
        flex bison build-essential make libncurses5-dev openssl libssl-dev \
        uuid-dev g++sl nasm iasl \
    	qemu
```

Besides, `qemu-system-x86` is also required. We recommend using version 8.0.4.

```bash
$ qemu-system-x86_64 -version
QEMU emulator version 8.0.4
Copyright (c) 2003-2022 Fabrice Bellard and the QEMU Project developers
```



## Setup

Clone this repository.

```bash
git clone https://github.com/XiuTsong/uefi-project1.git
```

Enter the directory `run-ovmf`, use the following command to compile edk2 and linux kernel.

```
make all
```



## Run

Also in directory  `run-ovmf`, use the following command to run qemu. 

```
make run-only
```

If you want to quit qemu, you can simply type `ctrl+a x` in your terminal. 

If you want to debug linux kernel, you can open one terminal and type the following command.

```
make run
```

And then you should `touch` a .gdbinit file in `run-ovmf`, just like the follows:

```
# .gdbinit
target remote localhost:1234
add-symbol-file ../linux/vmlinux
```

Open another terminal and type:

```
gdb -n -x .gdbinit
```

Now you can create some breakpoint in linux kernel and type `c` to continue.
