#ifndef __GPIO_CONTROL_H__
#define __GPIO_CONTROL_H__

#include <stdint.h>

/* GPIO Device Path */
#define GPIO_DEVICE_PATH "/dev/gpio_dev"

/* Function Prototypes */
int gpio_open_device(void);
int gpio_close_device(int fd);
int gpio_read_value(int fd, uint8_t *value);
int gpio_write_value(int fd, uint8_t value);
int gpio_set_direction(int fd, uint8_t direction);
int gpio_get_direction(int fd, uint8_t *direction);
void gpio_print_status(int fd);

#endif /* __GPIO_CONTROL_H__ */
