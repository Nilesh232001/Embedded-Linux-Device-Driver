/*
 * GPIO Device Driver - Main Driver Module
 * Platform Driver for GPIO Control with Character Device Interface
 * 
 * This module implements a Linux kernel character device driver that provides
 * user-space access to GPIO pins through Device Tree-based platform driver.
 *
 * License: GPL v2
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/platform_device.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/of_gpio.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>
#include <linux/spinlock.h>
#include <linux/mutex.h>
#include <linux/slab.h>
#include <linux/wait.h>
#include <asm/uaccess.h>

#include "../include/gpio_driver.h"

/* Module metadata */
MODULE_AUTHOR("Embedded Linux Developer");
MODULE_DESCRIPTION("GPIO Device Driver with Platform Driver and Character Device Interface");
MODULE_LICENSE("GPL v2");
MODULE_VERSION("1.0.0");

/* Global variables */
static dev_t gpio_device_num;                    /* Device number (major, minor) */
static struct cdev *gpio_cdev;                   /* Character device structure */
static struct class *gpio_class;                 /* Device class */
static struct device *gpio_device;               /* Device structure */
static struct gpio_device *gpio_dev_data;        /* Driver private data */
static DEFINE_MUTEX(gpio_mutex);                 /* Mutex for device access */

/* GPIO-specific variables */
static int gpio_number = 21;                     /* Default GPIO number (GPIO21 = BCM21 on RPi) */
static int gpio_irq = -1;                        /* IRQ number for GPIO */
static bool interrupt_triggered = false;         /* Interrupt flag */
static wait_queue_head_t gpio_wait_queue;        /* Wait queue for blocking reads */

/* Module parameters */
module_param(gpio_number, int, S_IRUGO | S_IWUSR);
MODULE_PARM_DESC(gpio_number, "GPIO number to control (default: 21)");

/*
 * GPIO Interrupt Service Routine
 * Called when GPIO interrupt is triggered
 */
static irqreturn_t gpio_interrupt_handler(int irq, void *dev_id)
{
    struct gpio_device *dev = (struct gpio_device *)dev_id;
    
    if (dev == NULL) {
        printk(KERN_ERR "GPIO_DRIVER: Invalid device pointer in ISR\n");
        return IRQ_NONE;
    }
    
    interrupt_triggered = true;
    printk(KERN_INFO "GPIO_DRIVER: Interrupt triggered on GPIO %d (IRQ %d)\n", 
           dev->gpio_number, irq);
    
    /* Wake up any process waiting in read() */
    wake_up_interruptible(&gpio_wait_queue);
    
    return IRQ_HANDLED;
}

/*
 * Character Device: Open
 * Called when /dev/gpio_dev is opened
 */
static int gpio_open(struct inode *inode, struct file *filp)
{
    struct gpio_device *dev;
    
    printk(KERN_INFO "GPIO_DRIVER: Device opened\n");
    
    /* Get device private data from cdev container_of */
    dev = container_of(inode->i_cdev, struct gpio_device, cdev);
    filp->private_data = dev;
    
    mutex_lock(&gpio_mutex);
    
    /* GPIO is already requested in probe(), just initialize flags */
    dev->value = gpio_get_value(dev->gpio_number);
    
    mutex_unlock(&gpio_mutex);
    
    return 0;
}

/*
 * Character Device: Release/Close
 * Called when /dev/gpio_dev is closed
 */
static int gpio_release(struct inode *inode, struct file *filp)
{
    struct gpio_device *dev = filp->private_data;
    
    if (dev == NULL) {
        printk(KERN_ERR "GPIO_DRIVER: Invalid device pointer in release\n");
        return -EINVAL;
    }
    
    printk(KERN_INFO "GPIO_DRIVER: Device closed\n");
    
    return 0;
}

/*
 * Character Device: Read
 * Read GPIO value from device
 * Returns 1 byte containing GPIO state (0 or 1)
 */
static ssize_t gpio_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{
    struct gpio_device *dev = filp->private_data;
    unsigned char gpio_value;
    int ret;
    
    if (dev == NULL) {
        printk(KERN_ERR "GPIO_DRIVER: Invalid device pointer in read\n");
        return -EINVAL;
    }
    
    if (count < 1) {
        printk(KERN_WARN "GPIO_DRIVER: Read count less than 1 byte\n");
        return -EINVAL;
    }
    
    mutex_lock(&gpio_mutex);
    
    /* Read current GPIO value */
    gpio_value = (unsigned char)gpio_get_value(dev->gpio_number);
    dev->value = gpio_value;
    
    printk(KERN_DEBUG "GPIO_DRIVER: Read GPIO %d value: %d\n", 
           dev->gpio_number, gpio_value);
    
    mutex_unlock(&gpio_mutex);
    
    /* Copy GPIO value to user buffer */
    ret = copy_to_user(buf, &gpio_value, 1);
    if (ret != 0) {
        printk(KERN_ERR "GPIO_DRIVER: Failed to copy data to user space\n");
        return -EFAULT;
    }
    
    *f_pos += 1;
    return 1;  /* Return number of bytes read */
}

/*
 * Character Device: Write
 * Write GPIO value to device
 * Expects 1 byte: 0 = Low, non-zero = High
 */
static ssize_t gpio_write(struct file *filp, const char __user *buf, 
                         size_t count, loff_t *f_pos)
{
    struct gpio_device *dev = filp->private_data;
    unsigned char gpio_value;
    int ret;
    
    if (dev == NULL) {
        printk(KERN_ERR "GPIO_DRIVER: Invalid device pointer in write\n");
        return -EINVAL;
    }
    
    if (count < 1) {
        printk(KERN_WARN "GPIO_DRIVER: Write count less than 1 byte\n");
        return -EINVAL;
    }
    
    /* Copy value from user buffer */
    ret = copy_from_user(&gpio_value, buf, 1);
    if (ret != 0) {
        printk(KERN_ERR "GPIO_DRIVER: Failed to copy data from user space\n");
        return -EFAULT;
    }
    
    mutex_lock(&gpio_mutex);
    
    /* Check if GPIO is configured as output */
    if (dev->direction != GPIO_DIRECTION_OUTPUT) {
        printk(KERN_WARN "GPIO_DRIVER: Cannot write to input GPIO\n");
        mutex_unlock(&gpio_mutex);
        return -EACCES;
    }
    
    /* Set GPIO value */
    gpio_set_value(dev->gpio_number, gpio_value ? 1 : 0);
    dev->value = gpio_value ? 1 : 0;
    
    printk(KERN_DEBUG "GPIO_DRIVER: Wrote GPIO %d value: %d\n", 
           dev->gpio_number, dev->value);
    
    mutex_unlock(&gpio_mutex);
    
    *f_pos += 1;
    return 1;  /* Return number of bytes written */
}

/*
 * Character Device: IOCTL
 * Handle device-specific I/O control commands
 */
static long gpio_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
    struct gpio_device *dev = filp->private_data;
    int ret = 0;
    int direction = 0;
    int value = 0;
    
    if (dev == NULL) {
        printk(KERN_ERR "GPIO_DRIVER: Invalid device pointer in ioctl\n");
        return -EINVAL;
    }
    
    if (_IOC_TYPE(cmd) != 'g') {
        printk(KERN_ERR "GPIO_DRIVER: Invalid ioctl magic number\n");
        return -ENOTTY;
    }
    
    mutex_lock(&gpio_mutex);
    
    switch (cmd) {
    case GPIO_IOCTL_SET_VALUE:
        /* Set GPIO output value */
        ret = copy_from_user(&value, (int __user *)arg, sizeof(int));
        if (ret != 0) {
            printk(KERN_ERR "GPIO_DRIVER: Failed to copy value from user\n");
            ret = -EFAULT;
            break;
        }
        
        if (dev->direction != GPIO_DIRECTION_OUTPUT) {
            printk(KERN_WARN "GPIO_DRIVER: Cannot set value on input GPIO\n");
            ret = -EACCES;
            break;
        }
        
        gpio_set_value(dev->gpio_number, value ? 1 : 0);
        dev->value = value ? 1 : 0;
        printk(KERN_INFO "GPIO_DRIVER: IOCTL SET_VALUE to %d\n", dev->value);
        break;
    
    case GPIO_IOCTL_GET_VALUE:
        /* Get GPIO current value */
        value = gpio_get_value(dev->gpio_number);
        dev->value = value;
        
        ret = copy_to_user((int __user *)arg, &value, sizeof(int));
        if (ret != 0) {
            printk(KERN_ERR "GPIO_DRIVER: Failed to copy value to user\n");
            ret = -EFAULT;
            break;
        }
        
        printk(KERN_INFO "GPIO_DRIVER: IOCTL GET_VALUE = %d\n", value);
        break;
    
    case GPIO_IOCTL_SET_DIRECTION:
        /* Set GPIO direction (input/output) */
        ret = copy_from_user(&direction, (int __user *)arg, sizeof(int));
        if (ret != 0) {
            printk(KERN_ERR "GPIO_DRIVER: Failed to copy direction from user\n");
            ret = -EFAULT;
            break;
        }
        
        if (direction == GPIO_DIRECTION_INPUT) {
            ret = gpio_direction_input(dev->gpio_number);
            if (ret == 0) {
                dev->direction = GPIO_DIRECTION_INPUT;
                printk(KERN_INFO "GPIO_DRIVER: Set GPIO to INPUT\n");
            } else {
                printk(KERN_ERR "GPIO_DRIVER: Failed to set GPIO to INPUT\n");
            }
        } else if (direction == GPIO_DIRECTION_OUTPUT) {
            ret = gpio_direction_output(dev->gpio_number, 0);
            if (ret == 0) {
                dev->direction = GPIO_DIRECTION_OUTPUT;
                printk(KERN_INFO "GPIO_DRIVER: Set GPIO to OUTPUT\n");
            } else {
                printk(KERN_ERR "GPIO_DRIVER: Failed to set GPIO to OUTPUT\n");
            }
        } else {
            printk(KERN_ERR "GPIO_DRIVER: Invalid direction value\n");
            ret = -EINVAL;
        }
        break;
    
    case GPIO_IOCTL_GET_DIRECTION:
        /* Get GPIO current direction */
        ret = copy_to_user((int __user *)arg, &dev->direction, sizeof(int));
        if (ret != 0) {
            printk(KERN_ERR "GPIO_DRIVER: Failed to copy direction to user\n");
            ret = -EFAULT;
            break;
        }
        
        printk(KERN_INFO "GPIO_DRIVER: IOCTL GET_DIRECTION = %d\n", dev->direction);
        break;
    
    default:
        printk(KERN_ERR "GPIO_DRIVER: Unknown ioctl command: 0x%X\n", cmd);
        ret = -ENOIOCTLCMD;
        break;
    }
    
    mutex_unlock(&gpio_mutex);
    
    return ret;
}

/*
 * File Operations Structure
 * Defines operations on /dev/gpio_dev character device
 */
static const struct file_operations gpio_fops = {
    .owner = THIS_MODULE,
    .open = gpio_open,
    .release = gpio_release,
    .read = gpio_read,
    .write = gpio_write,
    .unlocked_ioctl = gpio_ioctl,
};

/*
 * Platform Driver: Probe Function
 * Called when device matching compatible string is found in Device Tree
 */
static int gpio_probe(struct platform_device *pdev)
{
    int ret = 0;
    struct device_node *node = pdev->dev.of_node;
    
    printk(KERN_INFO "GPIO_DRIVER: Probe function called\n");
    
    if (node == NULL) {
        printk(KERN_ERR "GPIO_DRIVER: Device Tree node not found\n");
        return -ENODEV;
    }
    
    /* Allocate memory for device structure */
    gpio_dev_data = kzalloc(sizeof(struct gpio_device), GFP_KERNEL);
    if (gpio_dev_data == NULL) {
        printk(KERN_ERR "GPIO_DRIVER: Failed to allocate device memory\n");
        return -ENOMEM;
    }
    
    /* Get GPIO number from Device Tree */
    ret = of_property_read_u32(node, "gpio-number", (u32 *)&gpio_dev_data->gpio_number);
    if (ret != 0) {
        /* Use module parameter as fallback */
        gpio_dev_data->gpio_number = gpio_number;
        printk(KERN_INFO "GPIO_DRIVER: Using module parameter GPIO number: %d\n", gpio_number);
    } else {
        printk(KERN_INFO "GPIO_DRIVER: Got GPIO number from Device Tree: %d\n", 
               gpio_dev_data->gpio_number);
    }
    
    /* Request GPIO */
    ret = gpio_request(gpio_dev_data->gpio_number, DEVICE_NAME);
    if (ret != 0) {
        printk(KERN_ERR "GPIO_DRIVER: Failed to request GPIO %d (error: %d)\n", 
               gpio_dev_data->gpio_number, ret);
        kfree(gpio_dev_data);
        return ret;
    }
    
    printk(KERN_INFO "GPIO_DRIVER: GPIO %d requested successfully\n", 
           gpio_dev_data->gpio_number);
    
    /* Set default direction to output */
    ret = gpio_direction_output(gpio_dev_data->gpio_number, 0);
    if (ret != 0) {
        printk(KERN_ERR "GPIO_DRIVER: Failed to set GPIO direction\n");
        gpio_free(gpio_dev_data->gpio_number);
        kfree(gpio_dev_data);
        return ret;
    }
    
    gpio_dev_data->direction = GPIO_DIRECTION_OUTPUT;
    gpio_dev_data->value = 0;
    
    /* Store platform device as driver data */
    platform_set_drvdata(pdev, gpio_dev_data);
    
    /* Request IRQ for GPIO (if available in Device Tree) */
    gpio_irq = irq_of_parse_and_map(node, 0);
    if (gpio_irq > 0) {
        ret = request_irq(gpio_irq, gpio_interrupt_handler, 
                         IRQF_TRIGGER_FALLING, DEVICE_NAME, gpio_dev_data);
        if (ret == 0) {
            printk(KERN_INFO "GPIO_DRIVER: IRQ %d registered successfully\n", gpio_irq);
        } else {
            printk(KERN_WARN "GPIO_DRIVER: Failed to register IRQ (error: %d)\n", ret);
            gpio_irq = -1;
        }
    }
    
    /* Initialize character device */
    cdev_init(&gpio_dev_data->cdev, &gpio_fops);
    gpio_dev_data->cdev.owner = THIS_MODULE;
    
    /* Add character device to system */
    ret = cdev_add(&gpio_dev_data->cdev, gpio_device_num, 1);
    if (ret != 0) {
        printk(KERN_ERR "GPIO_DRIVER: Failed to add character device (error: %d)\n", ret);
        gpio_free(gpio_dev_data->gpio_number);
        if (gpio_irq > 0) {
            free_irq(gpio_irq, gpio_dev_data);
        }
        kfree(gpio_dev_data);
        return ret;
    }
    
    printk(KERN_INFO "GPIO_DRIVER: Character device added successfully\n");
    
    /* Create device node in /dev */
    gpio_device = device_create(gpio_class, &pdev->dev, gpio_device_num, NULL, DEVICE_NAME);
    if (IS_ERR(gpio_device)) {
        printk(KERN_ERR "GPIO_DRIVER: Failed to create device node\n");
        cdev_del(&gpio_dev_data->cdev);
        gpio_free(gpio_dev_data->gpio_number);
        if (gpio_irq > 0) {
            free_irq(gpio_irq, gpio_dev_data);
        }
        kfree(gpio_dev_data);
        return PTR_ERR(gpio_device);
    }
    
    printk(KERN_INFO "GPIO_DRIVER: Device node /dev/%s created successfully\n", DEVICE_NAME);
    printk(KERN_INFO "GPIO_DRIVER: Platform device probe completed successfully\n");
    
    return 0;
}

/*
 * Platform Driver: Remove Function
 * Called when device is removed or driver is unloaded
 */
static int gpio_remove(struct platform_device *pdev)
{
    struct gpio_device *dev = platform_get_drvdata(pdev);
    
    printk(KERN_INFO "GPIO_DRIVER: Remove function called\n");
    
    if (dev == NULL) {
        printk(KERN_ERR "GPIO_DRIVER: Invalid device pointer in remove\n");
        return -EINVAL;
    }
    
    /* Destroy device node */
    if (gpio_device != NULL) {
        device_destroy(gpio_class, gpio_device_num);
        gpio_device = NULL;
    }
    
    /* Delete character device */
    cdev_del(&dev->cdev);
    
    /* Free IRQ if allocated */
    if (gpio_irq > 0) {
        free_irq(gpio_irq, dev);
        gpio_irq = -1;
    }
    
    /* Free GPIO */
    gpio_free(dev->gpio_number);
    
    /* Free allocated memory */
    kfree(dev);
    gpio_dev_data = NULL;
    
    printk(KERN_INFO "GPIO_DRIVER: Device removed and cleanup completed\n");
    
    return 0;
}

/*
 * Device Tree Matching Table
 */
static const struct of_device_id gpio_of_match[] = {
    { .compatible = "custom,gpio-led" },
    { .compatible = "custom,gpio-buzzer" },
    { .compatible = "custom,gpio-button" },
    { /* sentinel */ },
};
MODULE_DEVICE_TABLE(of, gpio_of_match);

/*
 * Platform Driver Structure
 */
static struct platform_driver gpio_platform_driver = {
    .probe = gpio_probe,
    .remove = gpio_remove,
    .driver = {
        .name = "gpio_device_driver",
        .owner = THIS_MODULE,
        .of_match_table = of_match_ptr(gpio_of_match),
    },
};

/*
 * Module Initialization
 * Registers character device class and platform driver
 */
static int __init gpio_driver_init(void)
{
    int ret;
    
    printk(KERN_INFO "GPIO_DRIVER: Initializing GPIO Device Driver\n");
    
    /* Initialize wait queue for interrupt handling */
    init_waitqueue_head(&gpio_wait_queue);
    
    /* Allocate character device number (major, minor) */
    ret = alloc_chrdev_region(&gpio_device_num, 0, 1, DEVICE_NAME);
    if (ret < 0) {
        printk(KERN_ERR "GPIO_DRIVER: Failed to allocate character device number\n");
        return ret;
    }
    
    printk(KERN_INFO "GPIO_DRIVER: Character device number allocated: %d:%d\n",
           MAJOR(gpio_device_num), MINOR(gpio_device_num));
    
    /* Create device class */
    gpio_class = class_create(THIS_MODULE, CLASS_NAME);
    if (IS_ERR(gpio_class)) {
        printk(KERN_ERR "GPIO_DRIVER: Failed to create device class\n");
        unregister_chrdev_region(gpio_device_num, 1);
        return PTR_ERR(gpio_class);
    }
    
    printk(KERN_INFO "GPIO_DRIVER: Device class created successfully\n");
    
    /* Register platform driver */
    ret = platform_driver_register(&gpio_platform_driver);
    if (ret < 0) {
        printk(KERN_ERR "GPIO_DRIVER: Failed to register platform driver\n");
        class_destroy(gpio_class);
        unregister_chrdev_region(gpio_device_num, 1);
        return ret;
    }
    
    printk(KERN_INFO "GPIO_DRIVER: Platform driver registered successfully\n");
    printk(KERN_INFO "GPIO_DRIVER: Module initialized\n");
    
    return 0;
}

/*
 * Module Exit
 * Unregisters platform driver and frees resources
 */
static void __exit gpio_driver_exit(void)
{
    printk(KERN_INFO "GPIO_DRIVER: Exiting GPIO Device Driver\n");
    
    /* Unregister platform driver */
    platform_driver_unregister(&gpio_platform_driver);
    
    /* Destroy device class */
    class_destroy(gpio_class);
    
    /* Unregister character device region */
    unregister_chrdev_region(gpio_device_num, 1);
    
    printk(KERN_INFO "GPIO_DRIVER: Module exited\n");
}

/* Module entry and exit points */
module_init(gpio_driver_init);
module_exit(gpio_driver_exit);
