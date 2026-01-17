# System Architecture Documentation

## Overview

This document describes the architecture of the Embedded Linux GPIO Device Driver.

## Component Diagram

```
┌─────────────────────────────────────────────────────────────┐
│                    User Space Application                    │
│                      (gpio_app)                              │
└─────────────────────────────────────────────────────────────┘
                               │
                    ┌──────────┴──────────┐
                    │  open/close/read/   │
                    │  write/ioctl/mmap   │
                    └──────────┬──────────┘
                               │
┌─────────────────────────────────────────────────────────────┐
│                         /dev/gpio_dev                        │
└─────────────────────────────────────────────────────────────┘
                               │
┌─────────────────────────────────────────────────────────────┐
│                  Character Device Interface                  │
│  (file_operations: open, release, read, write, ioctl)       │
└─────────────────────────────────────────────────────────────┘
                               │
┌─────────────────────────────────────────────────────────────┐
│                   Platform Driver Interface                  │
│  (probe, remove, Device Tree matching)                       │
└─────────────────────────────────────────────────────────────┘
                               │
┌─────────────────────────────────────────────────────────────┐
│                    GPIO Hardware Control                     │
│  (GPIO library, IRQ handling, memory-mapped I/O)            │
└─────────────────────────────────────────────────────────────┘
                               │
┌─────────────────────────────────────────────────────────────┐
│                 GPIO Hardware (LED/Buzzer/Button)            │
└─────────────────────────────────────────────────────────────┘
```

## Driver Components

### 1. Character Device Driver (`gpio_char_dev.c`)
- Implements character device interface
- Handles open, release, read, write, ioctl operations
- Manages device file permissions and user-kernel communication

### 2. Platform Driver (`gpio_platform.c`)
- Matches devices using Device Tree
- Probes and initializes GPIO hardware
- Handles device removal and cleanup
- Manages interrupt handlers

### 3. GPIO Control Module (`gpio_driver.c`)
- Low-level GPIO operations
- Interrupt service routine (ISR) implementation
- Hardware register manipulation
- Kernel logging and error handling

## Data Flow

### Write Operation (User → Kernel)
1. User application calls `write(fd, buffer, count)`
2. Character device driver's write handler processes request
3. Platform driver updates GPIO state
4. GPIO hardware receives control signal

### Read Operation (Kernel → User)
1. User application calls `read(fd, buffer, count)`
2. Character device driver queries GPIO status
3. GPIO state is read from hardware
4. Data is copied to user buffer

### Interrupt Flow
1. GPIO hardware generates interrupt (edge/level triggered)
2. ISR is invoked by kernel
3. IRQ handler processes event
4. Event notification sent to user space (optional)

## Device Tree Integration

- Devices declared in `.dts` files
- Compatible strings match with platform driver
- GPIO numbers and interrupt configurations
- Enable device tree overlay support

## Synchronization Mechanisms

- Mutex locks for device access
- Spin locks for interrupt-safe operations
- Reference counting for device lifecycle

## System Calls

- `open()` - Device initialization
- `close()` - Cleanup and resource release
- `read()` - Read GPIO state
- `write()` - Control GPIO output
- `ioctl()` - Configuration and special commands
- `mmap()` (optional) - Memory-mapped I/O for performance

## Kernel Logging

All driver operations are logged using `printk()` with appropriate log levels:
- `KERN_INFO` - General information
- `KERN_DEBUG` - Debug messages
- `KERN_ERR` - Error conditions
- `KERN_WARN` - Warning messages

View logs with: `dmesg -w`
