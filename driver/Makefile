# Makefile for Linux GPIO Device Driver

# Kernel module name
obj-m += gpio_device_driver.o

# Source files
gpio_device_driver-objs := src/gpio_driver.o src/gpio_char_dev.o src/gpio_platform.o

# Kernel build directory (adjust for your system)
KDIR ?= /lib/modules/$(shell uname -r)/build

# Current directory
PWD := $(shell pwd)

# Default target
all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

# Clean build artifacts
clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
	rm -rf *.o *.ko *.mod.c *.symvers *.order

# Install module
install: all
	$(MAKE) -C $(KDIR) M=$(PWD) modules_install

# Load module
load: all
	insmod gpio_device_driver.ko

# Unload module
unload:
	rmmod gpio_device_driver

# View kernel messages
dmesg:
	dmesg -w

.PHONY: all clean install load unload dmesg
