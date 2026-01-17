# GETTING_STARTED.md - Quick Start Guide

## Quick Start

### Prerequisites
- Linux system with kernel source headers
- GCC compiler
- Build tools (make, gcc, etc.)
- Device Tree support (Raspberry Pi, BeagleBone, etc.)

### 1. Install Dependencies

#### Debian/Ubuntu
```bash
sudo apt-get update
sudo apt-get install -y \
    build-essential \
    linux-headers-$(uname -r) \
    device-tree-compiler \
    git
```

#### Fedora/RHEL
```bash
sudo dnf install -y \
    gcc \
    kernel-devel \
    make \
    device-tree-compiler \
    git
```

### 2. Clone or Download Project

```bash
cd ~/projects
git clone <repository-url>
cd Embedded-Linux-GPIO-Device-Driver
```

### 3. Automatic Setup (Recommended)

```bash
chmod +x setup.sh
./setup.sh
```

This will:
- Check system dependencies
- Install missing packages
- Build kernel module
- Build user application

### 4. Manual Build

#### Build Kernel Module
```bash
cd driver
make
sudo make load
```

#### Build User Application
```bash
cd user_app
make
```

### 5. Verify Installation

```bash
# Check if module is loaded
lsmod | grep gpio_device_driver

# Check device node
ls -l /dev/gpio_dev

# Check kernel messages
dmesg | grep GPIO_DRIVER
```

### 6. Run User Application

```bash
cd user_app

# Show help
./gpio_app help

# Read GPIO value
./gpio_app read

# Write GPIO value
./gpio_app write 1

# Blink LED
./gpio_app blink 5

# Interactive mode
./gpio_app interactive
```

## Common Operations

### Load/Unload Module

```bash
# Load module
sudo make -C driver load

# Unload module
sudo make -C driver unload

# Check if loaded
make -C driver check
```

### View Kernel Messages

```bash
# Show recent messages
dmesg | grep GPIO_DRIVER

# Monitor live
sudo dmesg -w | grep GPIO_DRIVER
```

### Interactive Mode Examples

```bash
$ ./gpio_app interactive

gpio> read
SUCCESS: Read GPIO value: 1

gpio> write 0
SUCCESS: Wrote GPIO value: 0

gpio> setdir 1
SUCCESS: Set GPIO direction: OUTPUT

gpio> blink 3
Blinking LED 3 times...

gpio> status
=== GPIO Status ===
Value: LOW (0)
Direction: OUTPUT
===================

gpio> exit
```

## Device Tree Configuration

### Raspberry Pi Example

Create `/boot/firmware/overlays/gpio-custom.dtb`:

```bash
# Edit dts/gpio-device.dts
dtc -I dts -O dtb -o gpio-device.dtbo dts/gpio-device.dts

# Copy to overlays directory
sudo cp gpio-device.dtbo /boot/firmware/overlays/

# Add to /boot/config.txt:
# dtoverlay=gpio-custom
```

### Generic Platform

Update Device Tree source in `dts/gpio-device.dts` with:
- GPIO numbers (21, 26, 27 for LED, Buzzer, Button)
- IRQ configurations
- GPIO modes (input/output)

## Hardware Connection Examples

### LED (GPIO 21)
```
Anode (+) ----[330Ω]---- GPIO 21
Cathode (-) ------------- GND
```

### Button (GPIO 27)
```
GPIO 27 ---- [Button] ---- GND
GPIO 27 ---- [Pull-up] ---- 3.3V (or use internal pull-up)
```

### Buzzer (GPIO 26)
```
GPIO 26 ---- [Buzzer +] ---- 5V
GND -------- [Buzzer -] ---- GND
```

## Troubleshooting

### Module fails to compile
```bash
# Update kernel headers
sudo apt-get install -y linux-headers-$(uname -r)

# Check kernel build directory
ls -la /lib/modules/$(uname -r)/build
```

### Cannot open /dev/gpio_dev
```bash
# Ensure module is loaded
sudo make -C driver load

# Check with
lsmod | grep gpio_device_driver
ls -l /dev/gpio_dev
```

### Permission denied errors
```bash
# Application needs device access permissions
# Either run with sudo or add user to dialout group
sudo usermod -a -G dialout $USER

# Then log out and back in
```

### GPIO already in use
```bash
# Check which process is using GPIO
cat /sys/kernel/debug/gpio

# Unload other GPIO drivers if needed
sudo rmmod gpio_*
```

## Documentation

- [README.md](README.md) - Project overview
- [BUILD_INSTRUCTIONS.md](docs/BUILD_INSTRUCTIONS.md) - Detailed build guide
- [ARCHITECTURE.md](docs/ARCHITECTURE.md) - System architecture
- [API_REFERENCE.md](docs/API_REFERENCE.md) - API documentation

## Support

For issues:
1. Check kernel messages: `dmesg -w | grep GPIO_DRIVER`
2. Verify module is loaded: `lsmod | grep gpio`
3. Check device node: `ls -l /dev/gpio_dev`
4. Review documentation in `docs/` folder

## Next Steps

- Review [ARCHITECTURE.md](docs/ARCHITECTURE.md) for system design
- Check [API_REFERENCE.md](docs/API_REFERENCE.md) for API usage
- Explore kernel code in `driver/src/`
- Examine user application in `user_app/src/`
- Modify Device Tree in `dts/` for your hardware
