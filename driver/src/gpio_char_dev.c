/*
 * GPIO Char Device Implementation
 * 
 * This file contains the character device interface implementation
 * for the GPIO device driver. It is compiled into gpio_device_driver.ko
 *
 * License: GPL v2
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/gpio.h>
#include <linux/mutex.h>
#include <asm/uaccess.h>

#include "../include/gpio_driver.h"

extern struct cdev gpio_cdev_instance;
extern struct class *gpio_device_class;
extern struct mutex gpio_device_mutex;

/* Stub file - main functionality is in gpio_driver.c */
printk(KERN_INFO "GPIO_DRIVER: Character device functions linked\n");
