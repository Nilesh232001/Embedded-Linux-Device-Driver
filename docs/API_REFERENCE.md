# GPIO Driver API Reference

## User Space API

### Device File Operations

#### Open Device
```c
int fd = open("/dev/gpio_dev", O_RDWR);
if (fd < 0) {
    perror("open");
}
```

#### Close Device
```c
close(fd);
```

#### Read GPIO Value
```c
unsigned char value;
ssize_t bytes_read = read(fd, &value, 1);
if (bytes_read < 0) {
    perror("read");
}
```

#### Write GPIO Value
```c
unsigned char value = 1;  // High
ssize_t bytes_written = write(fd, &value, 1);
if (bytes_written < 0) {
    perror("write");
}
```

#### IOCTL Operations

##### Set GPIO Value
```c
int value = 1;  // High or Low
int ret = ioctl(fd, GPIO_IOCTL_SET_VALUE, &value);
if (ret < 0) {
    perror("ioctl GPIO_IOCTL_SET_VALUE");
}
```

##### Get GPIO Value
```c
int value;
int ret = ioctl(fd, GPIO_IOCTL_GET_VALUE, &value);
if (ret == 0) {
    printf("GPIO Value: %d\n", value);
}
```

##### Set GPIO Direction
```c
// Direction: 0 = INPUT, 1 = OUTPUT
int direction = 1;  // OUTPUT
int ret = ioctl(fd, GPIO_IOCTL_SET_DIRECTION, &direction);
if (ret < 0) {
    perror("ioctl GPIO_IOCTL_SET_DIRECTION");
}
```

##### Get GPIO Direction
```c
int direction;
int ret = ioctl(fd, GPIO_IOCTL_GET_DIRECTION, &direction);
if (ret == 0) {
    printf("GPIO Direction: %s\n", direction ? "OUTPUT" : "INPUT");
}
```

### IOCTL Command Definitions

```c
#define GPIO_IOCTL_SET_VALUE       _IOW('g', 1, int)
#define GPIO_IOCTL_GET_VALUE       _IOR('g', 2, int)
#define GPIO_IOCTL_SET_DIRECTION   _IOW('g', 3, int)
#define GPIO_IOCTL_GET_DIRECTION   _IOR('g', 4, int)
```

## Kernel Space API

### Module Parameters

```c
module_param(gpio_number, int, S_IRUGO);
MODULE_PARM_DESC(gpio_number, "GPIO number to control");
```

### File Operations

```c
static const struct file_operations gpio_fops = {
    .owner = THIS_MODULE,
    .open = gpio_open,
    .release = gpio_release,
    .read = gpio_read,
    .write = gpio_write,
    .unlocked_ioctl = gpio_ioctl,
};
```

### Platform Driver

```c
static const struct of_device_id gpio_of_match[] = {
    { .compatible = "custom,gpio-led" },
    { .compatible = "custom,gpio-buzzer" },
    { .compatible = "custom,gpio-button" },
    { /* sentinel */ },
};
MODULE_DEVICE_TABLE(of, gpio_of_match);

static struct platform_driver gpio_platform_driver = {
    .probe = gpio_probe,
    .remove = gpio_remove,
    .driver = {
        .name = "gpio_driver",
        .of_match_table = gpio_of_match,
    },
};
```

### Interrupt Handler

```c
static irqreturn_t gpio_interrupt_handler(int irq, void *dev_id) {
    struct gpio_device *dev = (struct gpio_device *)dev_id;
    
    printk(KERN_INFO "GPIO interrupt detected on GPIO %d\n", dev->gpio_number);
    
    // Handle interrupt
    // ...
    
    return IRQ_HANDLED;
}
```

### GPIO Operations

```c
// Request GPIO
int ret = gpio_request(gpio_number, "gpio_dev");

// Set GPIO direction
int ret = gpio_direction_output(gpio_number, 1);  // Output, High
int ret = gpio_direction_input(gpio_number);      // Input

// Set GPIO value
gpio_set_value(gpio_number, 1);  // High

// Get GPIO value
int value = gpio_get_value(gpio_number);

// Free GPIO
gpio_free(gpio_number);
```

### Kernel Logging

```c
printk(KERN_INFO "Information message\n");
printk(KERN_DEBUG "Debug message\n");
printk(KERN_ERR "Error message\n");
printk(KERN_WARN "Warning message\n");
```

View logs:
```bash
dmesg -w
dmesg | grep gpio_driver
```

## Error Codes

| Code | Meaning |
|------|---------|
| -ENODEV | Device not found |
| -EBUSY | Resource busy |
| -EINVAL | Invalid argument |
| -EACCES | Permission denied |
| -ENOMEM | Out of memory |
| -EIO | Input/output error |
| -ENOTTY | Inappropriate ioctl |
| -ENOIOCTLCMD | Unknown ioctl command |

## Example Usage

### Read GPIO Status
```c
int fd = open("/dev/gpio_dev", O_RDWR);
unsigned char value;
read(fd, &value, 1);
printf("GPIO value: %d\n", value);
close(fd);
```

### Control LED
```c
int fd = open("/dev/gpio_dev", O_RDWR);
unsigned char on = 1;
write(fd, &on, 1);
sleep(2);
unsigned char off = 0;
write(fd, &off, 1);
close(fd);
```

### Interrupt-Based Button Monitoring
```c
int fd = open("/dev/gpio_dev", O_RDWR);

// Set direction to input
int dir = 0;  // INPUT
ioctl(fd, GPIO_IOCTL_SET_DIRECTION, &dir);

// Poll or use select/poll for interrupt notification
unsigned char value;
while(1) {
    read(fd, &value, 1);
    if (value) {
        printf("Button pressed!\n");
    }
    usleep(100000);  // 100ms delay
}

close(fd);
```

## References

- Linux Kernel GPIO Documentation: `/usr/src/linux/Documentation/gpio/`
- Device Tree Specification: devicetree.org
- Character Device Driver Guide
