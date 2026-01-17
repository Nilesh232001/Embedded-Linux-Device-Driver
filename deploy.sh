#!/bin/bash
# Deployment script for GPIO Device Driver
# Automates loading module and setting up environment

DRIVER_PATH="./driver/gpio_device_driver.ko"
APP_PATH="./user_app/gpio_app"

echo "GPIO Device Driver - Deployment"
echo "================================"

# Check if running as root
if [ "$(id -u)" != "0" ]; then
    echo "Error: This script must be run as root"
    echo "Usage: sudo $0"
    exit 1
fi

# Check if module exists
if [ ! -f "$DRIVER_PATH" ]; then
    echo "Error: Kernel module not found at $DRIVER_PATH"
    echo "Build it first with: make -C driver"
    exit 1
fi

# Unload previous version if loaded
if lsmod | grep -q gpio_device_driver; then
    echo "Removing previously loaded module..."
    rmmod gpio_device_driver
    sleep 1
fi

# Load the module
echo "Loading kernel module..."
insmod "$DRIVER_PATH"

if [ $? -ne 0 ]; then
    echo "Error: Failed to load kernel module"
    dmesg | tail -5
    exit 1
fi

echo "✓ Kernel module loaded successfully"

# Wait for device to be created
sleep 1

# Check device node
if [ -c /dev/gpio_dev ]; then
    echo "✓ Device node created: /dev/gpio_dev"
    ls -l /dev/gpio_dev
else
    echo "Warning: Device node not found, waiting for udev..."
    sleep 2
    if [ -c /dev/gpio_dev ]; then
        echo "✓ Device node created: /dev/gpio_dev"
    else
        echo "Warning: Device node may not be created yet"
    fi
fi

# Show module info
echo ""
echo "Module Information:"
echo "  Name: gpio_device_driver"
modinfo "$DRIVER_PATH" | grep -E "version|description|author"

# Show kernel messages
echo ""
echo "Recent kernel messages:"
dmesg | grep GPIO_DRIVER | tail -3 | sed 's/^/  /'

# Check if user application is available
if [ -x "$APP_PATH" ]; then
    echo ""
    echo "User application ready at: $APP_PATH"
    echo "Test with: $APP_PATH read"
else
    echo ""
    echo "Note: User application not found"
    echo "Build with: make -C user_app"
fi

echo ""
echo "Deployment complete!"
