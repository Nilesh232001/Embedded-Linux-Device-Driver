/*
 * GPIO User Space Application
 * 
 * Demonstrates GPIO control from user space using the GPIO device driver
 * Supports LED control, button monitoring, and buzzer control
 * 
 * Compile: gcc -Wall -O2 -I./include -o gpio_app src/main.c src/gpio_control.c
 * Usage: ./gpio_app [options]
 * 
 * License: GPL v2
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <time.h>

#include "gpio_control.h"

/* Global flag for signal handling */
static int keep_running = 1;

/* Signal handler for graceful shutdown */
static void signal_handler(int sig)
{
    printf("\n\nReceived signal %d - Exiting...\n", sig);
    keep_running = 0;
}

/*
 * Display help message
 */
void print_help(const char *program_name)
{
    printf("GPIO Device Driver User Application\n");
    printf("Usage: %s [COMMAND] [OPTION]\n\n", program_name);
    printf("Commands:\n");
    printf("  read            Read GPIO value once\n");
    printf("  write VALUE     Write VALUE to GPIO (0=Low, 1=High)\n");
    printf("  blink [COUNT]   Blink LED (default 10 times)\n");
    printf("  monitor [TIME]  Monitor GPIO for TIME seconds (default 10)\n");
    printf("  setdir DIR      Set GPIO direction (0=input, 1=output)\n");
    printf("  getdir          Get GPIO current direction\n");
    printf("  status          Show GPIO status\n");
    printf("  interactive     Interactive mode (menu-driven)\n");
    printf("  help            Display this help message\n");
    printf("\nExamples:\n");
    printf("  %s read\n", program_name);
    printf("  %s write 1\n", program_name);
    printf("  %s blink 5\n", program_name);
    printf("  %s monitor 20\n", program_name);
    printf("  %s interactive\n", program_name);
}

/*
 * Read GPIO value once
 */
int cmd_read_value(int fd)
{
    uint8_t value = 0;
    int ret;
    
    printf("Reading GPIO value...\n");
    ret = gpio_read_value(fd, &value);
    
    if (ret == 0) {
        printf("GPIO Value: %s\n", value ? "HIGH (1)" : "LOW (0)");
        return 0;
    }
    
    return -1;
}

/*
 * Write GPIO value
 */
int cmd_write_value(int fd, int value)
{
    printf("Writing GPIO value: %d\n", value);
    
    if (gpio_set_direction(fd, 1) != 0) {  /* Set to output */
        fprintf(stderr, "ERROR: Failed to set GPIO direction to output\n");
        return -1;
    }
    
    return gpio_write_value(fd, (uint8_t)value);
}

/*
 * Blink LED (toggle GPIO on and off)
 */
int cmd_blink_led(int fd, int count)
{
    int i;
    
    printf("Blinking LED %d times...\n", count);
    
    if (gpio_set_direction(fd, 1) != 0) {  /* Set to output */
        fprintf(stderr, "ERROR: Failed to set GPIO direction to output\n");
        return -1;
    }
    
    for (i = 0; i < count && keep_running; i++) {
        printf("Blink %d: ON\n", i + 1);
        if (gpio_write_value(fd, 1) != 0) {
            return -1;
        }
        
        sleep(1);
        
        if (!keep_running) break;
        
        printf("Blink %d: OFF\n", i + 1);
        if (gpio_write_value(fd, 0) != 0) {
            return -1;
        }
        
        sleep(1);
    }
    
    printf("LED blinking complete\n");
    return 0;
}

/*
 * Monitor GPIO value for changes
 */
int cmd_monitor_gpio(int fd, int duration)
{
    uint8_t prev_value = 0xFF;  /* Invalid initial value */
    uint8_t curr_value = 0;
    time_t start_time, current_time;
    int ret;
    
    printf("Monitoring GPIO for %d seconds (press Ctrl+C to stop)...\n", duration);
    
    if (gpio_set_direction(fd, 0) != 0) {  /* Set to input */
        fprintf(stderr, "ERROR: Failed to set GPIO direction to input\n");
        return -1;
    }
    
    time(&start_time);
    
    while (keep_running) {
        time(&current_time);
        
        if (duration > 0 && (current_time - start_time) >= duration) {
            printf("Monitoring time elapsed\n");
            break;
        }
        
        ret = gpio_read_value(fd, &curr_value);
        if (ret != 0) {
            return -1;
        }
        
        if (curr_value != prev_value) {
            printf("GPIO value changed: %s\n", curr_value ? "HIGH (1)" : "LOW (0)");
            prev_value = curr_value;
        }
        
        usleep(100000);  /* 100ms polling interval */
    }
    
    printf("Monitoring stopped\n");
    return 0;
}

/*
 * Set GPIO direction
 */
int cmd_set_direction(int fd, int direction)
{
    printf("Setting GPIO direction to: %s\n", direction ? "OUTPUT" : "INPUT");
    return gpio_set_direction(fd, (uint8_t)direction);
}

/*
 * Get GPIO direction
 */
int cmd_get_direction(int fd)
{
    uint8_t direction = 0;
    int ret;
    
    ret = gpio_get_direction(fd, &direction);
    if (ret == 0) {
        printf("Current GPIO direction: %s\n", direction ? "OUTPUT" : "INPUT");
    }
    
    return ret;
}

/*
 * Show GPIO status
 */
int cmd_show_status(int fd)
{
    gpio_print_status(fd);
    return 0;
}

/*
 * Interactive mode menu
 */
int interactive_mode(int fd)
{
    char command[256];
    uint8_t value = 0;
    uint8_t direction = 0;
    int count = 0;
    
    printf("\n=== GPIO Interactive Mode ===\n");
    printf("Type 'help' for available commands\n\n");
    
    while (keep_running) {
        printf("gpio> ");
        fflush(stdout);
        
        if (fgets(command, sizeof(command), stdin) == NULL) {
            break;
        }
        
        /* Remove newline */
        size_t len = strlen(command);
        if (len > 0 && command[len-1] == '\n') {
            command[len-1] = '\0';
        }
        
        if (strlen(command) == 0) {
            continue;
        }
        
        if (strcmp(command, "help") == 0) {
            printf("\nAvailable commands:\n");
            printf("  read              - Read GPIO value\n");
            printf("  write <val>       - Write value (0 or 1)\n");
            printf("  blink <count>     - Blink LED\n");
            printf("  setdir <dir>      - Set direction (0=input, 1=output)\n");
            printf("  getdir            - Get GPIO direction\n");
            printf("  status            - Show GPIO status\n");
            printf("  monitor <sec>     - Monitor GPIO\n");
            printf("  exit              - Exit interactive mode\n");
            printf("  help              - Show this help\n\n");
        }
        else if (strcmp(command, "read") == 0) {
            cmd_read_value(fd);
        }
        else if (strncmp(command, "write ", 6) == 0) {
            value = atoi(command + 6);
            cmd_write_value(fd, value);
        }
        else if (strncmp(command, "blink", 5) == 0) {
            count = 5;  /* Default count */
            if (strlen(command) > 5 && command[5] == ' ') {
                count = atoi(command + 6);
            }
            cmd_blink_led(fd, count);
        }
        else if (strncmp(command, "setdir ", 7) == 0) {
            direction = atoi(command + 7);
            cmd_set_direction(fd, direction);
        }
        else if (strcmp(command, "getdir") == 0) {
            cmd_get_direction(fd);
        }
        else if (strcmp(command, "status") == 0) {
            cmd_show_status(fd);
        }
        else if (strncmp(command, "monitor", 7) == 0) {
            int time_val = 10;  /* Default 10 seconds */
            if (strlen(command) > 7 && command[7] == ' ') {
                time_val = atoi(command + 8);
            }
            cmd_monitor_gpio(fd, time_val);
        }
        else if (strcmp(command, "exit") == 0) {
            printf("Exiting interactive mode\n");
            break;
        }
        else {
            printf("Unknown command: '%s'. Type 'help' for commands\n", command);
        }
        
        printf("\n");
    }
    
    return 0;
}

/*
 * Main function
 */
int main(int argc, char *argv[])
{
    int fd;
    int ret = 0;
    
    /* Set up signal handlers */
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
    
    printf("GPIO Device Driver User Application v1.0\n");
    printf("=========================================\n\n");
    
    /* Open GPIO device */
    fd = gpio_open_device();
    if (fd < 0) {
        fprintf(stderr, "FATAL: Cannot open GPIO device\n");
        return 1;
    }
    
    /* Parse command line arguments */
    if (argc < 2) {
        print_help(argv[0]);
        gpio_close_device(fd);
        return 0;
    }
    
    if (strcmp(argv[1], "help") == 0) {
        print_help(argv[0]);
    }
    else if (strcmp(argv[1], "read") == 0) {
        ret = cmd_read_value(fd);
    }
    else if (strcmp(argv[1], "write") == 0) {
        if (argc < 3) {
            fprintf(stderr, "ERROR: write command requires a value argument\n");
            ret = -1;
        } else {
            ret = cmd_write_value(fd, atoi(argv[2]));
        }
    }
    else if (strcmp(argv[1], "blink") == 0) {
        int count = 10;  /* Default 10 blinks */
        if (argc >= 3) {
            count = atoi(argv[2]);
        }
        ret = cmd_blink_led(fd, count);
    }
    else if (strcmp(argv[1], "monitor") == 0) {
        int duration = 10;  /* Default 10 seconds */
        if (argc >= 3) {
            duration = atoi(argv[2]);
        }
        ret = cmd_monitor_gpio(fd, duration);
    }
    else if (strcmp(argv[1], "setdir") == 0) {
        if (argc < 3) {
            fprintf(stderr, "ERROR: setdir command requires direction argument\n");
            ret = -1;
        } else {
            ret = cmd_set_direction(fd, atoi(argv[2]));
        }
    }
    else if (strcmp(argv[1], "getdir") == 0) {
        ret = cmd_get_direction(fd);
    }
    else if (strcmp(argv[1], "status") == 0) {
        ret = cmd_show_status(fd);
    }
    else if (strcmp(argv[1], "interactive") == 0) {
        ret = interactive_mode(fd);
    }
    else {
        fprintf(stderr, "Unknown command: '%s'\n", argv[1]);
        print_help(argv[0]);
        ret = 1;
    }
    
    /* Close GPIO device */
    gpio_close_device(fd);
    
    printf("\nApplication terminated (exit code: %d)\n", ret);
    
    return (ret == 0) ? 0 : 1;
}
