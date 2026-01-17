# Makefile for Linux GPIO Device Driver
# Builds a loadable kernel module (LKM) for GPIO control
# Supports Device Tree-based platform driver with character device interface

# Kernel module name
obj-m += gpio_device_driver.o

# Source files for the kernel module
# The kernel build system will combine these into gpio_device_driver.ko
gpio_device_driver-objs := src/gpio_driver.o

# Kernel build directory (adjust for your system)
KDIR ?= /lib/modules/$(shell uname -r)/build

# Current directory
PWD := $(shell pwd)

# Compiler flags for debugging and development
EXTRA_CFLAGS += -Wall -Wextra -O2 -DDEBUG

# Default target - builds the kernel module
all: modules

# Build kernel modules
modules:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

# Clean build artifacts
clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
	rm -f src/*.o src/*.cmd *.ko *.mod *.symvers *.order .Module.symvers
	rm -rf .tmp_versions

# Install module (requires root)
install: all
	$(MAKE) -C $(KDIR) M=$(PWD) modules_install
	depmod -a

# Load module into kernel (requires root)
load: all
	sudo insmod gpio_device_driver.ko

# Unload module from kernel (requires root)
unload:
	sudo rmmod gpio_device_driver

# Check if module is loaded
check:
	lsmod | grep gpio_device_driver || echo "Module not loaded"

# View module information
info:
	modinfo gpio_device_driver.ko

# View kernel log messages related to GPIO driver
dmesg:
	dmesg | grep -i gpio || echo "No GPIO messages found"

# Continuously monitor kernel messages
dmesg-live:
	sudo dmesg -w | grep -i gpio

# Remove kernel messages buffer
clear-dmesg:
	sudo dmesg -c

# Complete setup: clean, build, and load
setup: clean all load check

# Help message
help:
	@echo "GPIO Device Driver - Makefile targets:"
	@echo "  make              - Build the kernel module"
	@echo "  make modules      - Build kernel modules"
	@echo "  make clean        - Clean build artifacts"
	@echo "  make install      - Install module (needs root)"
	@echo "  make load         - Load module into kernel (needs root)"
	@echo "  make unload       - Unload module from kernel (needs root)"
	@echo "  make check        - Check if module is loaded"
	@echo "  make info         - Show module information"
	@echo "  make dmesg        - Show GPIO kernel messages"
	@echo "  make dmesg-live   - Monitor kernel messages live"
	@echo "  make clear-dmesg  - Clear kernel message buffer"
	@echo "  make setup        - Build and load module"
	@echo "  make help         - Show this help message"

.PHONY: all modules clean install load unload check info dmesg dmesg-live clear-dmesg setup help
