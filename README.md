# Embedded Linux GPIO Device Driver (Character + Platform Driver)

## 📌 Overview

This project implements a **Linux Kernel GPIO Device Driver** using a
**Platform Driver combined with a Character Device interface**. It
enables **user-space applications** to control and monitor **GPIO-based
hardware** (LED / Buzzer / Button) through standard Linux system calls.

The driver is **Device Tree--based**, supports **interrupt handling**,
and demonstrates **real-world Embedded Linux kernel development
practices**.

------------------------------------------------------------------------

## 🎯 Objectives

-   Develop a Linux **character device driver**
-   Use **Device Tree** for hardware abstraction
-   Implement **open, read, write, ioctl** system calls
-   Handle **GPIO interrupts**
-   Enable **user--kernel space communication**

------------------------------------------------------------------------

## 🧠 System Architecture

User Space App → `/dev/gpio_dev` → Kernel (Character + Platform Driver)
→ GPIO Hardware

------------------------------------------------------------------------

## ✨ Features

-   Platform driver matched using Device Tree
-   Dynamic creation of `/dev/gpio_dev`
-   GPIO control using read/write/ioctl
-   Interrupt handling with ISR
-   Kernel logging using dmesg

------------------------------------------------------------------------

## 📂 Project Structure

embedded-linux-gpio-driver/ - driver/ - user_app/ - dts/ - README.md

------------------------------------------------------------------------

## ⚙️ Device Tree Example

``` dts
gpio_led: gpio_led@0 {
    compatible = "custom,gpio-led";
    led-gpio = <&gpio 21 GPIO_ACTIVE_HIGH>;
    interrupts = <21 IRQ_TYPE_EDGE_FALLING>;
};
```

------------------------------------------------------------------------

## 🛠 Tools & Technologies

Linux Kernel, C, Device Tree, GPIO, IRQ, Makefile, Git

------------------------------------------------------------------------

## 📄 Resume Description

Developed an Embedded Linux GPIO device driver using platform and
character driver architecture with Device Tree and interrupt handling.

------------------------------------------------------------------------

## 👤 Author

Nilesh Patil
