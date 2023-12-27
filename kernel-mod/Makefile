TARGET=km_example
obj-m:=$(TARGET).o
CURRENT_PATH:=$(shell pwd)
LINUX_KERNEL:=$(shell uname -r)
# LINUX_KERNEL_PATH:=/home/liu/Desktop/uefi-project/linux
LINUX_KERNEL_PATH:=/usr/src/linux-headers-$(LINUX_KERNEL)

all:
	make -C $(LINUX_KERNEL_PATH) M=$(CURRENT_PATH) modules
clean:
	make -C $(LINUX_KERNEL_PATH) M=$(CURRENT_PATH) clean
test:
	sudo dmesg -C
	sudo insmod km_example.ko
	sudo rmmod km_example.ko
	dmesg