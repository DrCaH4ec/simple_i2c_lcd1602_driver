# (!) using paths with spaces may not work with Kbuild

TARGET ?= lcd_driver
TEST_FILE ?= ./testfiles/test

OBJS += lcd.o cdev.o

# this is what is used by Kbuild
obj-m += $(TARGET)_mod.o
$(TARGET)_mod-objs += $(TARGET).o $(OBJS)


V ?= 2

# directory containing Makefile for kernel build
KBUILDDIR ?= ~/BBB/kernel/linux-5.10.14
REMDIR ?= /srv/nfs/busybox/modules
PWD = $(shell pwd)

# Do not print "Entering directory" on recursive make call if not verbose
MAKEFLAGS += $(if $(value V),,--no-print-directory)

EXTRA_CFLAGS += -std=gnu11 -O2

export PATH := /opt/gcc-arm-10.2-2020.11-x86_64-arm-none-linux-gnueabihf/bin/:$(PATH)
export CROSS_COMPILE := arm-none-linux-gnueabihf-
#export ARCH := arm	

.PHONY: modules clean tidy run module_deploy test_deploy check_patch set_env format 
.PHONY: build_and_deploy test_build_and_deploy test_tidy

# recur to the original kernel Makefile with some additions
modules: tidy
	$(MAKE) -C "$(KBUILDDIR)" M="$(PWD)" V=$(V) modules

tidy:
	$(MAKE) -C "$(KBUILDDIR)" M="$(PWD)" V=$(V) clean

# we link original clean to target named tidy
clean:
	-rm -rf .tmp_versions
	-rm -f modules.order .modules.order.cmd Module.symvers .Module.symvers.cmd
	-rm -f $(obj-m) $(obj-m:.o=.mod) $(obj-m:.o=.mod.o) $(obj-m:.o=.mod.c) .$(obj-m:.o=.mod.cmd)
	-rm -f $(addsuffix .cmd,$(addprefix .,$(obj-m)))
	-rm -f $(addsuffix .cmd,$(addprefix .,$(obj-m:.o=.ko)))
	-rm -f $(addsuffix .cmd,$(addprefix .,$(obj-m:.o=.mod.o)))

module_deploy:
	sudo cp $(TARGET)_mod.ko $(REMDIR)

test_deploy:
	sudo cp $(TEST_FILE) $(REMDIR)


set_env:
	export PATH=/opt/gcc-arm-10.2-2020.11-x86_64-arm-none-linux-gnueabihf/bin/:$$PATH
	export CROSS_COMPILE=arm-none-linux-gnueabihf-


check_patch:
	$(KBUILDDIR)/scripts/checkpatch.pl -f --no-tree $(TARGET).c


format:
	clang-format -verbose -i $(TARGET).c -style=file


build_and_deploy: | modules module_deploy


test_build:
	arm-none-linux-gnueabihf-gcc $(TEST_FILE).c -o $(TEST_FILE)


test_build_and_deploy: | test_build test_deploy


test_tidy:
	rm -f $(TEST_FILE)
