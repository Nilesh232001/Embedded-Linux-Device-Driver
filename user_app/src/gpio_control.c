/*
 * GPIO Control Library
 * 
 * Provides high-level functions for GPIO control from user space
 * 
 * License: GPL v2
 */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <string.h>
#include <errno.h>

#include "gpio_control.h"

/* Macro for IOCTL commands - mirrors kernel definitions */
#define GPIO_IOCTL_SET_VALUE    _IOW('g', 1, int)
#define GPIO_IOCTL_GET_VALUE    _IOR('g', 2, int)
#define GPIO_IOCTL_SET_DIRECTION _IOW('g', 3, int)
#define GPIO_IOCTL_GET_DIRECTION _IOR('g', 4, int)

/* GPIO Direction constants */
#define GPIO_DIRECTION_INPUT    0
#define GPIO_DIRECTION_OUTPUT   1

/* GPIO Value constants */
#define GPIO_VALUE_LOW          0
#define GPIO_VALUE_HIGH         1

/*
 * gpio_open_device
 * 
 * Opens the GPIO device file
 * 
 * Returns: File descriptor on success, -1 on error
 */
int gpio_open_device(void)
{
    int fd;
    
    fd = open(GPIO_DEVICE_PATH, O_RDWR);
    if (fd < 0) {
        fprintf(stderr, "ERROR: Cannot open %s: %s\n", GPIO_DEVICE_PATH, strerror(errno));
        return -1;
    }
    
    printf("SUCCESS: Opened GPIO device: %s (fd=%d)\n", GPIO_DEVICE_PATH, fd);
    
    return fd;
}

/*
 * gpio_close_device
 * 
 * Closes the GPIO device file
 * 
 * Parameters:
 *   fd - File descriptor
 * 
 * Returns: 0 on success, -1 on error
 */
int gpio_close_device(int fd)
{
    if (fd < 0) {
        fprintf(stderr, "ERROR: Invalid file descriptor: %d\n", fd);
        return -1;
    }
    
    if (close(fd) < 0) {
        fprintf(stderr, "ERROR: Cannot close GPIO device: %s\n", strerror(errno));
        return -1;
    }
    
    printf("SUCCESS: Closed GPIO device\n");
    
    return 0;
}

/*
 * gpio_read_value
 * 
 * Reads the current GPIO value
 * 
 * Parameters:
 *   fd    - File descriptor
 *   value - Pointer to store the value (0 or 1)
 * 
 * Returns: 0 on success, -1 on error
 */
int gpio_read_value(int fd, uint8_t *value)
{
    ssize_t ret;
    unsigned char gpio_val;
    
    if (fd < 0) {
        fprintf(stderr, "ERROR: Invalid file descriptor: %d\n", fd);
        return -1;
    }
    
    if (value == NULL) {
        fprintf(stderr, "ERROR: Invalid value pointer\n");
        return -1;
    }
    
    ret = read(fd, &gpio_val, 1);
    if (ret < 0) {
        fprintf(stderr, "ERROR: Cannot read GPIO value: %s\n", strerror(errno));
        return -1;
    }
    
    if (ret != 1) {
        fprintf(stderr, "ERROR: Unexpected read size: %zd bytes\n", ret);
        return -1;
    }
    
    *value = gpio_val;
    printf("SUCCESS: Read GPIO value: %d\n", *value);
    
    return 0;
}

/*
 * gpio_write_value
 * 
 * Writes a value to the GPIO (must be configured as output)
 * 
 * Parameters:
 *   fd    - File descriptor
 *   value - Value to write (0=Low, 1=High)
 * 
 * Returns: 0 on success, -1 on error
 */
int gpio_write_value(int fd, uint8_t value)
{
    ssize_t ret;
    unsigned char gpio_val = (value ? 1 : 0);
    
    if (fd < 0) {
        fprintf(stderr, "ERROR: Invalid file descriptor: %d\n", fd);
        return -1;
    }
    
    ret = write(fd, &gpio_val, 1);
    if (ret < 0) {
        fprintf(stderr, "ERROR: Cannot write GPIO value: %s\n", strerror(errno));
        return -1;
    }
    
    if (ret != 1) {
        fprintf(stderr, "ERROR: Unexpected write size: %zd bytes\n", ret);
        return -1;
    }
    
    printf("SUCCESS: Wrote GPIO value: %d\n", gpio_val);
    
    return 0;
}

/*
 * gpio_set_direction
 * 
 * Sets GPIO direction (input or output)
 * 
 * Parameters:
 *   fd        - File descriptor
 *   direction - 0=Input, 1=Output
 * 
 * Returns: 0 on success, -1 on error
 */
int gpio_set_direction(int fd, uint8_t direction)
{
    int ret;
    int dir_val = (direction ? GPIO_DIRECTION_OUTPUT : GPIO_DIRECTION_INPUT);
    
    if (fd < 0) {
        fprintf(stderr, "ERROR: Invalid file descriptor: %d\n", fd);
        return -1;
    }
    
    ret = ioctl(fd, GPIO_IOCTL_SET_DIRECTION, &dir_val);
    if (ret < 0) {
        fprintf(stderr, "ERROR: Cannot set GPIO direction: %s\n", strerror(errno));
        return -1;
    }
    
    printf("SUCCESS: Set GPIO direction: %s\n", 
           direction ? "OUTPUT" : "INPUT");
    
    return 0;
}

/*
 * gpio_get_direction
 * 
 * Gets current GPIO direction
 * 
 * Parameters:
 *   fd        - File descriptor
 *   direction - Pointer to store direction (0=Input, 1=Output)
 * 
 * Returns: 0 on success, -1 on error
 */
int gpio_get_direction(int fd, uint8_t *direction)
{
    int ret;
    int dir_val;
    
    if (fd < 0) {
        fprintf(stderr, "ERROR: Invalid file descriptor: %d\n", fd);
        return -1;
    }
    
    if (direction == NULL) {
        fprintf(stderr, "ERROR: Invalid direction pointer\n");
        return -1;
    }
    
    ret = ioctl(fd, GPIO_IOCTL_GET_DIRECTION, &dir_val);
    if (ret < 0) {
        fprintf(stderr, "ERROR: Cannot get GPIO direction: %s\n", strerror(errno));
        return -1;
    }
    
    *direction = (dir_val == GPIO_DIRECTION_OUTPUT) ? 1 : 0;
    printf("SUCCESS: Got GPIO direction: %s\n", 
           *direction ? "OUTPUT" : "INPUT");
    
    return 0;
}

/*
 * gpio_print_status
 * 
 * Prints the current GPIO status (value and direction)
 * 
 * Parameters:
 *   fd - File descriptor
 */
void gpio_print_status(int fd)
{
    int ret;
    uint8_t value = 0;
    uint8_t direction = 0;
    
    if (fd < 0) {
        fprintf(stderr, "ERROR: Invalid file descriptor: %d\n", fd);
        return;
    }
    
    printf("\n=== GPIO Status ===\n");
    
    ret = gpio_read_value(fd, &value);
    if (ret == 0) {
        printf("Value: %s\n", value ? "HIGH (1)" : "LOW (0)");
    } else {
        printf("Value: <error reading>\n");
    }
    
    ret = gpio_get_direction(fd, &direction);
    if (ret == 0) {
        printf("Direction: %s\n", direction ? "OUTPUT" : "INPUT");
    } else {
        printf("Direction: <error reading>\n");
    }
    
    printf("===================\n\n");
}
