/*
 * GPIO Platform Driver Implementation
 * 
 * This file contains platform driver interface implementation
 * for the GPIO device driver. It is compiled into gpio_device_driver.ko
 *
 * License: GPL v2
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/platform_device.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/gpio.h>

#include "../include/gpio_driver.h"

/* Stub file - main functionality is in gpio_driver.c */
printk(KERN_INFO "GPIO_DRIVER: Platform driver functions linked\n");
