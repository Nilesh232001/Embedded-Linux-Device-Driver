/*
 * GPIO Device Driver Example Usage
 * 
 * This file demonstrates how to use the GPIO device driver from user space.
 * 
 * Compilation:
 *   gcc -Wall -O2 -o gpio_example gpio_example.c
 * 
 * Usage:
 *   sudo ./gpio_example
 */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <string.h>
#include <errno.h>

/* IOCTL command definitions */
#define GPIO_IOCTL_SET_VALUE    _IOW('g', 1, int)
#define GPIO_IOCTL_GET_VALUE    _IOR('g', 2, int)
#define GPIO_IOCTL_SET_DIRECTION _IOW('g', 3, int)
#define GPIO_IOCTL_GET_DIRECTION _IOR('g', 4, int)

#define GPIO_DIRECTION_INPUT    0
#define GPIO_DIRECTION_OUTPUT   1

#define GPIO_DEVICE_PATH "/dev/gpio_dev"

/* Error handling macro */
#define ERROR_CHECK(ret, msg) \
    if (ret < 0) { \
        fprintf(stderr, "ERROR: %s (errno: %s)\n", msg, strerror(errno)); \
        return -1; \
    }

/*
 * Example 1: Basic LED Control
 * Turns LED on and off
 */
int example_led_control()
{
    int fd, ret;
    int value;
    
    printf("\n=== Example 1: LED Control ===\n");
    
    /* Open GPIO device */
    fd = open(GPIO_DEVICE_PATH, O_RDWR);
    ERROR_CHECK(fd, "Cannot open GPIO device");
    printf("✓ Device opened\n");
    
    /* Set direction to output */
    int dir = GPIO_DIRECTION_OUTPUT;
    ret = ioctl(fd, GPIO_IOCTL_SET_DIRECTION, &dir);
    ERROR_CHECK(ret, "Cannot set direction to output");
    printf("✓ GPIO set to OUTPUT\n");
    
    /* Turn LED on */
    value = 1;
    ret = write(fd, (unsigned char*)&value, 1);
    ERROR_CHECK(ret, "Cannot write to GPIO");
    printf("✓ LED turned ON\n");
    sleep(2);
    
    /* Turn LED off */
    value = 0;
    ret = write(fd, (unsigned char*)&value, 1);
    ERROR_CHECK(ret, "Cannot write to GPIO");
    printf("✓ LED turned OFF\n");
    
    /* Close device */
    close(fd);
    printf("✓ Device closed\n");
    
    return 0;
}

/*
 * Example 2: LED Blinking
 * Blinks LED multiple times
 */
int example_blink_led()
{
    int fd, ret, i;
    unsigned char value;
    
    printf("\n=== Example 2: LED Blinking ===\n");
    
    fd = open(GPIO_DEVICE_PATH, O_RDWR);
    ERROR_CHECK(fd, "Cannot open GPIO device");
    
    /* Set to output */
    int dir = GPIO_DIRECTION_OUTPUT;
    ioctl(fd, GPIO_IOCTL_SET_DIRECTION, &dir);
    
    /* Blink 5 times */
    for (i = 0; i < 5; i++) {
        printf("Blink %d: ", i+1);
        
        /* On */
        value = 1;
        write(fd, &value, 1);
        printf("ON");
        fflush(stdout);
        sleep(1);
        
        /* Off */
        value = 0;
        write(fd, &value, 1);
        printf(" -> OFF\n");
        sleep(1);
    }
    
    close(fd);
    printf("✓ Blinking complete\n");
    
    return 0;
}

/*
 * Example 3: Button Monitoring
 * Reads button state continuously
 */
int example_button_monitoring()
{
    int fd, ret, i;
    unsigned char value;
    unsigned char prev_value = 0xFF;
    
    printf("\n=== Example 3: Button Monitoring ===\n");
    
    fd = open(GPIO_DEVICE_PATH, O_RDWR);
    ERROR_CHECK(fd, "Cannot open GPIO device");
    
    /* Set to input */
    int dir = GPIO_DIRECTION_INPUT;
    ioctl(fd, GPIO_IOCTL_SET_DIRECTION, &dir);
    
    printf("Monitoring for 10 seconds...\n");
    
    /* Poll for 10 seconds */
    for (i = 0; i < 20; i++) {  /* 20 x 0.5 sec = 10 sec */
        ret = read(fd, &value, 1);
        ERROR_CHECK(ret, "Cannot read from GPIO");
        
        if (value != prev_value) {
            printf("Button state changed: %s\n", 
                   value ? "PRESSED" : "RELEASED");
            prev_value = value;
        }
        
        usleep(500000);  /* 500ms sleep */
    }
    
    close(fd);
    printf("✓ Monitoring complete\n");
    
    return 0;
}

/*
 * Example 4: IOCTL Commands
 * Demonstrates IOCTL operations
 */
int example_ioctl_commands()
{
    int fd, ret, value, dir;
    
    printf("\n=== Example 4: IOCTL Commands ===\n");
    
    fd = open(GPIO_DEVICE_PATH, O_RDWR);
    ERROR_CHECK(fd, "Cannot open GPIO device");
    
    /* Set direction to output */
    dir = GPIO_DIRECTION_OUTPUT;
    ret = ioctl(fd, GPIO_IOCTL_SET_DIRECTION, &dir);
    printf("✓ SET_DIRECTION: %s\n", ret == 0 ? "OK" : "FAILED");
    
    /* Get direction */
    ret = ioctl(fd, GPIO_IOCTL_GET_DIRECTION, &dir);
    printf("✓ GET_DIRECTION: %d (%s)\n", dir, 
           dir == GPIO_DIRECTION_OUTPUT ? "OUTPUT" : "INPUT");
    
    /* Set value */
    value = 1;
    ret = ioctl(fd, GPIO_IOCTL_SET_VALUE, &value);
    printf("✓ SET_VALUE: %s\n", ret == 0 ? "OK" : "FAILED");
    
    /* Get value */
    ret = ioctl(fd, GPIO_IOCTL_GET_VALUE, &value);
    printf("✓ GET_VALUE: %d\n", value);
    
    close(fd);
    
    return 0;
}

/*
 * Example 5: PWM Simulation (software PWM)
 * Simulates PWM by rapid on/off cycles
 */
int example_pwm_simulation()
{
    int fd, i, j;
    unsigned char value;
    int duty_cycle = 75;  /* 75% duty cycle */
    int period = 100;      /* 100 cycles */
    
    printf("\n=== Example 5: PWM Simulation (75%% duty) ===\n");
    
    fd = open(GPIO_DEVICE_PATH, O_RDWR);
    if (fd < 0) {
        fprintf(stderr, "ERROR: Cannot open GPIO device\n");
        return -1;
    }
    
    /* Set to output */
    int dir = GPIO_DIRECTION_OUTPUT;
    ioctl(fd, GPIO_IOCTL_SET_DIRECTION, &dir);
    
    printf("Running PWM simulation for 2 seconds...\n");
    
    /* Simulate PWM for ~2 seconds */
    for (i = 0; i < 40; i++) {  /* 40 periods */
        /* LED ON for 75% of period */
        for (j = 0; j < (period * duty_cycle / 100); j++) {
            value = 1;
            write(fd, &value, 1);
            usleep(500);
        }
        
        /* LED OFF for 25% of period */
        for (j = 0; j < (period * (100 - duty_cycle) / 100); j++) {
            value = 0;
            write(fd, &value, 1);
            usleep(500);
        }
    }
    
    close(fd);
    printf("✓ PWM simulation complete\n");
    
    return 0;
}

/*
 * Example 6: Error Handling
 * Demonstrates error handling scenarios
 */
int example_error_handling()
{
    int fd, ret;
    int invalid_direction = 99;
    
    printf("\n=== Example 6: Error Handling ===\n");
    
    /* Try to open non-existent device */
    fd = open("/dev/nonexistent", O_RDWR);
    if (fd < 0) {
        printf("✓ Correctly handled non-existent device\n");
    }
    
    /* Open valid device */
    fd = open(GPIO_DEVICE_PATH, O_RDWR);
    if (fd < 0) {
        printf("ERROR: GPIO device not found\n");
        printf("Make sure kernel module is loaded:\n");
        printf("  sudo insmod ../driver/gpio_device_driver.ko\n");
        return -1;
    }
    
    /* Try invalid IOCTL */
    ret = ioctl(fd, 0xFFFFFFFF, NULL);
    if (ret < 0) {
        printf("✓ Correctly rejected invalid IOCTL\n");
    }
    
    /* Try invalid direction */
    ret = ioctl(fd, GPIO_IOCTL_SET_DIRECTION, &invalid_direction);
    if (ret < 0) {
        printf("✓ Correctly rejected invalid direction\n");
    }
    
    close(fd);
    printf("✓ Error handling complete\n");
    
    return 0;
}

/*
 * Main function
 * Runs all examples
 */
int main()
{
    printf("╔════════════════════════════════════════╗\n");
    printf("║  GPIO Device Driver - Usage Examples   ║\n");
    printf("╚════════════════════════════════════════╝\n");
    
    /* Run examples */
    if (example_led_control() != 0) goto cleanup;
    if (example_blink_led() != 0) goto cleanup;
    if (example_ioctl_commands() != 0) goto cleanup;
    if (example_error_handling() != 0) {
        printf("\nNote: Some examples require the kernel module to be loaded.\n");
        printf("Load it with: sudo insmod ../driver/gpio_device_driver.ko\n");
        return 1;
    }
    
    printf("\n✓ All examples completed successfully!\n\n");
    
    return 0;

cleanup:
    printf("\nError occurred in examples\n");
    return 1;
}
