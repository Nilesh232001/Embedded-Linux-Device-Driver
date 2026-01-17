# QUICK REFERENCE GUIDE

## 🚀 One-Line Commands

### Setup & Build
```bash
chmod +x *.sh && ./setup.sh          # Complete automatic setup
./build.sh build                      # Build everything
./build.sh driver                     # Build kernel module only
./build.sh app                        # Build app only
```

### Load/Unload
```bash
sudo make -C driver load              # Load module
sudo make -C driver unload            # Unload module
sudo make -C driver check             # Check if loaded
```

### Run Application
```bash
./user_app/gpio_app help              # Show help
./user_app/gpio_app read              # Read GPIO
./user_app/gpio_app write 1           # Write GPIO high
./user_app/gpio_app blink 5           # Blink 5 times
./user_app/gpio_app interactive       # Interactive mode
./user_app/gpio_app monitor 10        # Monitor for 10 sec
```

### Monitoring
```bash
dmesg | grep GPIO_DRIVER              # View logs
sudo dmesg -w | grep GPIO_DRIVER      # Live monitoring
sudo ./test.sh                        # Run test suite
```

### Cleanup
```bash
make -C driver clean                  # Clean driver
make -C user_app clean                # Clean app
./build.sh clean                      # Clean all
```

---

## 📋 File Locations

| File | Purpose |
|------|---------|
| `driver/src/gpio_driver.c` | Main kernel driver (600+ lines) |
| `user_app/src/main.c` | User application (400+ lines) |
| `user_app/src/gpio_control.c` | Control library (200+ lines) |
| `examples/gpio_example.c` | Usage examples |
| `docs/ARCHITECTURE.md` | System design |
| `docs/BUILD_INSTRUCTIONS.md` | Build guide |
| `docs/API_REFERENCE.md` | API docs |
| `dts/gpio-device.dts` | Device Tree overlay |

---

## 🔧 Common Tasks

### Task: Change GPIO Number
1. Edit `dts/gpio-device.dts` - Change gpio-number property
2. Edit driver/Makefile if needed - Adjust KDIR
3. Rebuild: `./build.sh clean && ./build.sh build`
4. Reload: `sudo ./build.sh load`

### Task: Add New GPIO Mode
1. Add new IOCTL define in `driver/include/gpio_driver.h`
2. Implement handler in `driver/src/gpio_driver.c`
3. Add user function in `user_app/src/gpio_control.c`
4. Update `user_app/src/main.c` with new command
5. Document in `docs/API_REFERENCE.md`

### Task: Cross-Compile for ARM
```bash
cd driver
export ARCH=arm
export CROSS_COMPILE=arm-linux-gnueabihf-
export KDIR=/path/to/arm/kernel/build
make
```

### Task: Debug Driver Issues
```bash
# View recent messages
dmesg | tail -20 | grep GPIO_DRIVER

# Monitor live
sudo dmesg -w | grep GPIO_DRIVER

# Load with debug output
sudo insmod driver/gpio_device_driver.ko gpio_number=21

# Run test suite
sudo ./test.sh
```

---

## 📊 GPIO Mapping Reference

### Raspberry Pi
| GPIO | Physical Pin | Common Use |
|------|--------------|-----------|
| GPIO21 | Pin 40 | LED |
| GPIO26 | Pin 37 | Buzzer |
| GPIO27 | Pin 13 | Button |

### BeagleBone Black
| GPIO | Device Tree | Common Use |
|------|-------------|-----------|
| GPIO1_28 | P9_12 | LED |
| GPIO1_16 | P9_15 | Button |
| GPIO0_23 | P8_13 | Buzzer |

---

## 🎯 IOCTL Commands

### C Code Examples

```c
// Set output value
int value = 1;
ioctl(fd, GPIO_IOCTL_SET_VALUE, &value);

// Get value
ioctl(fd, GPIO_IOCTL_GET_VALUE, &value);

// Set direction to output
int dir = 1;  // 1=output, 0=input
ioctl(fd, GPIO_IOCTL_SET_DIRECTION, &dir);

// Get direction
ioctl(fd, GPIO_IOCTL_GET_DIRECTION, &dir);
```

---

## 🐛 Troubleshooting Quick Guide

### Problem: `insmod: ERROR: could not insert module`
**Solution**: 
- Check kernel version matches: `uname -r`
- Install kernel headers: `sudo apt-get install linux-headers-$(uname -r)`
- Clean and rebuild: `make -C driver clean && make -C driver`

### Problem: `Cannot open /dev/gpio_dev`
**Solution**:
- Check module loaded: `lsmod | grep gpio_device_driver`
- Check device exists: `ls -l /dev/gpio_dev`
- Reload: `sudo make -C driver unload && sudo make -C driver load`

### Problem: Permission denied
**Solution**:
- Run with sudo: `sudo ./user_app/gpio_app read`
- Or add user to group and logout/login
- Check permissions: `ls -l /dev/gpio_dev`

### Problem: GPIO in use / already requested
**Solution**:
- Unload other GPIO modules: `sudo rmmod gpio_*`
- Check in use: `cat /sys/kernel/debug/gpio`
- Reboot system and try again

---

## 📖 Documentation Quick Links

- 🚀 **Start Here**: [GETTING_STARTED.md](GETTING_STARTED.md)
- 🏗️ **Build Guide**: [BUILD_INSTRUCTIONS.md](docs/BUILD_INSTRUCTIONS.md)
- 🧠 **Architecture**: [ARCHITECTURE.md](docs/ARCHITECTURE.md)
- 📚 **API Reference**: [API_REFERENCE.md](docs/API_REFERENCE.md)
- ✅ **Checklist**: [IMPLEMENTATION_CHECKLIST.md](IMPLEMENTATION_CHECKLIST.md)
- 📊 **Summary**: [PROJECT_SUMMARY.md](PROJECT_SUMMARY.md)

---

## 🧪 Test Commands

```bash
# Run full test suite
sudo ./test.sh

# Test specific functionality
sudo ./user_app/gpio_app read          # Test read
sudo ./user_app/gpio_app write 1       # Test write
sudo ./user_app/gpio_app blink 3       # Test blink
sudo ./user_app/gpio_app status        # Test status

# Compile and run examples
cd examples
gcc -Wall -O2 -o gpio_example gpio_example.c
sudo ./gpio_example
```

---

## 📋 Build Process Overview

```
Source Code
    ↓
    ├── driver/Makefile
    │   └── kernel build system
    │       └── gpio_device_driver.ko
    │
    ├── user_app/Makefile
    │   └── gcc compiler
    │       └── gpio_app
    │
    └── dts/
        └── Device Tree Compiler
            └── gpio-device.dtbo
```

---

## 🎓 Learning Path

1. **Read**: [GETTING_STARTED.md](GETTING_STARTED.md) - Overview
2. **Setup**: Run `./setup.sh` - Automatic setup
3. **Build**: Run `./build.sh build` - Build project
4. **Test**: Run `sudo ./test.sh` - Verify functionality
5. **Explore**: Read [ARCHITECTURE.md](docs/ARCHITECTURE.md)
6. **Code Review**: Examine `driver/src/gpio_driver.c`
7. **Examples**: Check `examples/gpio_example.c`
8. **Advanced**: Read [API_REFERENCE.md](docs/API_REFERENCE.md)

---

## 🔑 Key Files & Their Purpose

| File | Lines | Purpose |
|------|-------|---------|
| `gpio_driver.c` | 600+ | Main driver implementation |
| `main.c` | 400+ | User application CLI |
| `gpio_control.c` | 200+ | Control library functions |
| `gpio_driver.h` | 100+ | Structures and definitions |
| Makefiles | 200+ | Build configuration |
| Scripts | 800+ | Automation and testing |
| Documentation | 2000+ | Guides and reference |

---

## ✅ Verification Checklist

After setup, verify these work:

- [ ] `make -C driver` compiles without errors
- [ ] `make -C user_app` compiles without errors
- [ ] `sudo make -C driver load` loads module
- [ ] `lsmod | grep gpio_device_driver` shows loaded
- [ ] `./user_app/gpio_app read` works
- [ ] `dmesg | grep GPIO_DRIVER` shows messages
- [ ] `sudo ./test.sh` passes tests
- [ ] `./user_app/gpio_app interactive` starts

If all pass ✅ - System is properly configured!

---

## 🚀 Getting Help

**Quick Support Checklist**:
1. Check troubleshooting section above
2. Review relevant documentation file
3. Run `./test.sh` to identify issues
4. Check kernel messages: `dmesg | grep GPIO_DRIVER`
5. Verify Device Tree: `cat /proc/device-tree/...`
6. Review example code: `examples/gpio_example.c`

---

## 📞 Command Reference

### Build Commands
```bash
make -C driver              # Build driver
make -C user_app            # Build app
make -C driver clean        # Clean driver
make -C user_app clean      # Clean app
./build.sh build            # Build all via script
```

### Deploy Commands
```bash
sudo make -C driver load    # Load module
sudo make -C driver unload  # Unload module
sudo ./deploy.sh            # Deploy via script
```

### User App Commands
```bash
./user_app/gpio_app read           # Read GPIO
./user_app/gpio_app write 1        # Write GPIO
./user_app/gpio_app blink 5        # Blink LED
./user_app/gpio_app monitor 10     # Monitor GPIO
./user_app/gpio_app interactive    # Interactive mode
./user_app/gpio_app help           # Show help
```

### Monitoring Commands
```bash
dmesg -w | grep GPIO_DRIVER        # Monitor live
dmesg | grep GPIO_DRIVER           # View history
lsmod | grep gpio_device_driver    # Check loaded
ls -l /dev/gpio_dev                # Check device
./test.sh                          # Run tests
```

---

**Last Updated**: January 18, 2026
**Version**: 1.0.0
**Status**: Complete ✓
