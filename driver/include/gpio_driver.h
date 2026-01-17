#ifndef __GPIO_DRIVER_H__
#define __GPIO_DRIVER_H__

#include <linux/types.h>

/* Device name and node */
#define DEVICE_NAME "gpio_dev"
#define CLASS_NAME "gpio_class"

/* IOCTL commands */
#define GPIO_IOCTL_SET_VALUE    _IOW('g', 1, int)
#define GPIO_IOCTL_GET_VALUE    _IOR('g', 2, int)
#define GPIO_IOCTL_SET_DIRECTION _IOW('g', 3, int)
#define GPIO_IOCTL_GET_DIRECTION _IOR('g', 4, int)

/* GPIO Direction */
#define GPIO_DIRECTION_INPUT    0
#define GPIO_DIRECTION_OUTPUT   1

/* GPIO Value */
#define GPIO_VALUE_LOW          0
#define GPIO_VALUE_HIGH         1

/* Device private structure */
struct gpio_device {
    int gpio_number;
    int direction;
    int value;
    struct cdev cdev;
    struct device *device;
};

#endif /* __GPIO_DRIVER_H__ */
