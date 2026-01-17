# Build Instructions

## Prerequisites

### System Requirements
- Linux kernel headers for your kernel version
- GCC compiler (gcc/arm-linux-gcc for cross-compilation)
- Make utility
- Device Tree compiler (dtc)
- Git (for version control)

### Installation on Different Platforms

#### Debian/Ubuntu
```bash
sudo apt-get update
sudo apt-get install -y build-essential linux-headers-$(uname -r) device-tree-compiler
```

#### Fedora/RHEL
```bash
sudo dnf install -y gcc kernel-devel make device-tree-compiler
```

#### Raspberry Pi
```bash
sudo apt-get install -y raspberrypi-kernel-headers build-essential device-tree-compiler
```

## Building the Kernel Module

### 1. Navigate to Driver Directory
```bash
cd driver
```

### 2. Build the Module
```bash
make
```

This will compile:
- `gpio_driver.c` - Core driver functionality
- `gpio_char_dev.c` - Character device interface
- `gpio_platform.c` - Platform driver interface

### 3. Output Files
- `gpio_device_driver.ko` - Loadable kernel module
- `*.o` - Object files
- `*.mod.c` - Module metadata

## Building User Space Application

### 1. Navigate to User App Directory
```bash
cd user_app
```

### 2. Build the Application
```bash
make
```

### 3. Output
- `gpio_app` - Executable user-space application

## Compiling Device Tree

### 1. Navigate to Device Tree Directory
```bash
cd dts
```

### 2. Compile Device Tree Source
```bash
dtc -I dts -O dtb -o gpio-device.dtb gpio-device.dts
```

Or compile as Device Tree Overlay:
```bash
dtc -@ -I dts -O dtb -o gpio-device.dtbo gpio-device.dts
```

## Cross-Compilation (ARM/ARM64)

### For ARM 32-bit
```bash
cd driver
export ARCH=arm
export CROSS_COMPILE=arm-linux-gnueabihf-
export KDIR=/path/to/kernel/build
make
```

### For ARM 64-bit
```bash
cd driver
export ARCH=arm64
export CROSS_COMPILE=aarch64-linux-gnu-
export KDIR=/path/to/kernel/build
make
```

## Installing the Module

### 1. Load Module Temporarily (for testing)
```bash
cd driver
sudo insmod gpio_device_driver.ko
```

### 2. Verify Module Loading
```bash
lsmod | grep gpio_device_driver
dmesg | tail -20  # Check kernel messages
```

### 3. Permanent Installation
```bash
cd driver
sudo make install
sudo depmod -a
sudo modprobe gpio_device_driver
```

### 4. Unload Module
```bash
sudo rmmod gpio_device_driver
```

## Cleaning Build Files

### Clean Driver Build
```bash
cd driver
make clean
```

### Clean User App Build
```bash
cd user_app
make clean
```

### Clean All
```bash
cd ..
rm -rf build/*
cd driver && make clean
cd ../user_app && make clean
```

## Troubleshooting

### Build Errors

**Error: "kernel headers not found"**
```bash
# Install kernel headers
sudo apt-get install linux-headers-$(uname -r)
# Or set KDIR explicitly
make KDIR=/path/to/kernel/build
```

**Error: "gcc: command not found"**
```bash
sudo apt-get install build-essential
```

**Error: "device-tree-compiler not found"**
```bash
sudo apt-get install device-tree-compiler
```

### Module Loading Issues

**Error: "Unknown symbol in module"**
- Check kernel version compatibility
- Rebuild with matching kernel headers

**Error: "Device or resource busy"**
- Module may already be loaded
- Use `lsmod` to check
- Unload with `rmmod` first

## Verification

### Check Module Information
```bash
modinfo gpio_device_driver.ko
```

### Check Device Node
```bash
ls -l /dev/gpio_dev
cat /proc/devices | grep gpio_class
```

### Monitor Driver Activity
```bash
sudo dmesg -w | grep gpio_driver
```

## Build Configuration

Edit `driver/Makefile` to customize:
- Module name (`obj-m`)
- Source files (`gpio_device_driver-objs`)
- Compiler flags (`EXTRA_CFLAGS`)
- Kernel directory (`KDIR`)

Example:
```makefile
EXTRA_CFLAGS += -DDEBUG -O2
```

## Automated Build Script

Create `build.sh`:
```bash
#!/bin/bash
set -e

echo "Building GPIO Device Driver..."
cd driver
make clean
make

echo "Building User Application..."
cd ../user_app
make clean
make

echo "Build completed successfully!"
ls -la driver/*.ko
ls -la user_app/gpio_app
```

Run with: `bash build.sh`
