# Embedded Linux GPIO Device Driver

A comprehensive Linux Kernel GPIO Device Driver implementation using Platform Driver combined with a Character Device interface for controlling GPIO-based hardware.

## 🎯 Objectives

- Develop a Linux character device driver
- Use Device Tree for hardware abstraction
- Implement open, read, write, ioctl system calls
- Handle GPIO interrupts
- Enable user-kernel space communication

## 📂 Project Structure

```
embedded-linux-gpio-driver/
├── driver/                 # Kernel module source
│   ├── src/               # Driver implementation
│   ├── include/           # Driver headers
│   └── Makefile
├── user_app/              # User-space application
│   ├── src/               # Application source
│   ├── include/           # Application headers
│   └── Makefile
├── dts/                   # Device Tree files
├── build/                 # Build artifacts
├── docs/                  # Documentation
└── README.md
```

## ✨ Features

- Platform driver matched using Device Tree
- Dynamic creation of `/dev/gpio_dev`
- GPIO control using read/write/ioctl
- Interrupt handling with ISR
- Kernel logging using dmesg
- User-space GPIO control application

## 🧠 System Architecture

```
User Space App → /dev/gpio_dev → Kernel (Character + Platform Driver) → GPIO Hardware
```

## 🛠 Tools & Technologies

- Linux Kernel
- C Programming Language
- Device Tree
- GPIO Control
- IRQ Handling
- Makefile
- Git

## 🚀 Getting Started

### Prerequisites
- Linux kernel headers installed
- GCC compiler
- Device tree compiler (dtc)
- Make utility

### Building the Driver

```bash
cd driver
make
```

### Building the User Application

```bash
cd user_app
make
```

### Device Tree Configuration

Update the device tree file in `dts/` directory with your GPIO configuration:

```
gpio_led: gpio_led@0 {
    compatible = "custom,gpio-led";
    led-gpio = <&gpio 21 GPIO_ACTIVE_HIGH>;
    interrupts = <21 IRQ_TYPE_EDGE_FALLING>;
};
```

## 📝 License

This project is for educational purposes.

## 👤 Author

Your Name

## 📞 Support

For issues and questions, please refer to the documentation in the `docs/` folder.
