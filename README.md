# uefi-project
## Prerequesites

​	(If you have done labs in class before, this section can be skipped.)

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
git clone https://github.com/XiuTsong/uefi-project.git
```

Update edk2 submodule. 

```
git submodule update --recursive --init
```

Configure edk2(You can refer to the previous slides given by TA).

```
cd edk2
make -C BaseTools
source edksetup.sh BaseTools
cd ..

# you will find there are file edk2/Conf/target.txt
# edit this file, set the following setting.
#
# ACTIVE_PLATFORM  = OvmfPkg/OvmfPkgX64.dsc
# TARGET_ARCH      = X64
# TOOL_CHAIN_TAG   = GCC5
```

Configure linux

```
cd linux
cp myconfig .config
```

Configure busybos

```
cd busybox/_install
sudo mkdir dev 
sudo mknod dev/console c 5 1
sudo mknod dev/ram b 1 0
# You could also add your .bin file to _install
```

Enter the directory `run-ovmf`, use the following command to compile edk2 and linux kernel.

```
make all
```

Besides, you can compile edk2, linux and initramfss separately.

```
# Compile edk2
make edk2
# Compile linux
make linux
# Compile busybox
make initramfs
```

If you have problem compiling edk2, it is likely that edk2 is not fully cloned. You can first delete the edk2 folder with `rm - rf`, and then clone it with the following command:

```
git clone --recursive git@github.com:XiuTsong/uefi-edk2.git edk2
cd edk2
git pull
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


## modify the syscall
### usr mode (test/syscall.c)
可执行文件位于busybox/_install/syscall，即进入kernel后根目录的
syscall文件，用法为./syscall $service_name $paras(任意数量)。

该文件由test/syscall.c静态编译而来，一般无需修改。

若确需修改，请参照一下命令使其应用于kernel中。
```
cd test
gcc -static -o syscall syscall.c
rm ../busybox/_install/syscall
cp syscall ../busybox/_install
cd ../run-ovmf
make initramfs
```

### kernel 
在kernel内核中，如需添加对新服务的调用或修改对老服务的调用方式，泽只需要对该文件(linux/kernel/sys.c)下图部分进行修改。
```c
//comm为传递进来的service_name,枚举比较其名称（time_key）
  // add service here
  if (!strncmp(comm, "time_key", 10)) {
    efi_status_t status;
    pr_info("test_efi begin.\n");
    ...
  }
```
对所传递参数的取用例子
```c
//kargs[i]为第i个参数
pr_info("test_efi para %d: %s.\n", i, kargs[i]);
```