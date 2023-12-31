cur_dir:=$(CURDIR)
ROOT:=$(shell dirname $(cur_dir))

################################################################################
# Targets
################################################################################
TARGET_DEPS := linux edk2
TARGET_CLEAN := linux-clean edk2-clean

all: $(TARGET_DEPS)
clean: $(TARGET_CLEAN)

################################################################################
# Paths to git projects and various binaries
################################################################################
EDK2_PATH		?= $(ROOT)/edk2
EDK2_BUILD      ?= $(EDK2_PATH)/Build/OvmfX64/DEBUG_GCC5
EDK2_BIN		?= $(EDK2_BUILD)/FV/OVMF.fd
LINUX_PATH      ?= $(ROOT)/linux
LINUX_IMAGE     ?= $(LINUX_PATH)/arch/x86_64/boot/bzImage
BUSYBOX_PATH    ?= $(ROOT)/busybox/_install
EDKFS_PATH      ?= $(ROOT)/test

linux:
	make -C $(LINUX_PATH) -j16 

linux-clean:
	make clean -C $(LINUX_PATH)

edk2:
	cd $(EDK2_PATH) && \
	make -C BaseTools && \
	. $(EDK2_PATH)/edksetup.sh BaseTools && \
	build

edk2-clean:
	cd $(EDK2_PATH) && \
	. $(EDK2_PATH)/edksetup.sh BaseTools && \
	build clean

initramfs:
	# cd $(EDKFS_PATH) && \
	# gcc -static edkfs.c -o edkfs && \
	# cp edkfs $(BUSYBOX_PATH) \
	cd $(BUSYBOX_PATH) && \
	find . -print0 | cpio --null -ov --format=newc | gzip -9 > $(CURDIR)/initramfs-busybox-x64.cpio.gz

fs:
	gcc -static $(EDKFS_PATH)/edkfs.c -o $(BUSYBOX_PATH)/edkfs

################################################################################
# Qemu run
################################################################################

clean-debug:
	-rm $(EDK2_BUILD)/X64/*.debug

run-only: clean-debug
	qemu-system-x86_64 -kernel $(LINUX_IMAGE) \
	  -initrd $(cur_dir)/initramfs-busybox-x64.cpio.gz \
	  --bios $(EDK2_BIN) \
	  --debugcon file:"ovmf-boot.log" \
	  -global isa-debugcon.iobase=0x402 \
	  -global ICH9-LPC.disable_s3=1 \
	  -nographic \
	  -append "root=/dev/ram init=/init console=ttyS0 efi=debug" \
	  -m 512M \
	  -drive file=fat:rw:$(EDK2_BUILD)/X64,id=fat32,format=raw \

run: clean-debug
	qemu-system-x86_64  -s -S \
	  -kernel $(LINUX_IMAGE) \
	  -initrd $(cur_dir)/initramfs-busybox-x64.cpio.gz \
	  --bios $(EDK2_BIN) \
	  --debugcon file:"ovmf-boot.log" \
	  -global isa-debugcon.iobase=0x402 \
	  -global ICH9-LPC.disable_s3=1 \
	  -nographic \
	  -append "root=/dev/ram init=/init console=ttyS0 efi=debug" \
	  -m 512M \
	  -drive file=fat:rw:$(EDK2_BUILD)/X64,id=fat32,format=raw

lhz-run-only: clean-debug
	/home/moenosnkun/Downloads/qemu-8.0.4/build/qemu-system-x86_64 -kernel $(LINUX_IMAGE) \
	  -initrd $(cur_dir)/initramfs-busybox-x64.cpio.gz \
	  --bios $(EDK2_BIN) \
	  --debugcon file:"ovmf-boot.log" \
	  -global isa-debugcon.iobase=0x402 \
	  -global ICH9-LPC.disable_s3=1 \
	  -nographic \
	  -append "root=/dev/ram init=/init console=ttyS0 efi=debug" \
	  -m 512M \
	  -drive file=fat:rw:$(EDK2_BUILD)/X64,id=fat32,format=raw \

lhz-run: clean-debug
	/home/moenosnkun/Downloads/qemu-8.0.4/build/qemu-system-x86_64  -s -S \
	  -kernel $(LINUX_IMAGE) \
	  -initrd $(cur_dir)/initramfs-busybox-x64.cpio.gz \
	  --bios $(EDK2_BIN) \
	  --debugcon file:"ovmf-boot.log" \
	  -global isa-debugcon.iobase=0x402 \
	  -global ICH9-LPC.disable_s3=1 \
	  -nographic \
	  -append "root=/dev/ram init=/init console=ttyS0 efi=debug" \
	  -m 512M \
	  -drive file=fat:rw:$(EDK2_BUILD)/X64,id=fat32,format=raw



