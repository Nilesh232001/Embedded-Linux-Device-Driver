#!/bin/bash
# GPIO Device Driver - Build and Deploy Script
# This script automates the build process for both kernel module and user application

set -e  # Exit on any error

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m'  # No Color

PROJECT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
DRIVER_DIR="$PROJECT_DIR/driver"
APP_DIR="$PROJECT_DIR/user_app"
BUILD_DIR="$PROJECT_DIR/build"

# Function to print colored messages
print_info() {
    echo -e "${GREEN}[INFO]${NC} $1"
}

print_warn() {
    echo -e "${YELLOW}[WARN]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Function to check command existence
command_exists() {
    command -v "$1" >/dev/null 2>&1
}

# Function to display usage
usage() {
    cat <<EOF
GPIO Device Driver - Build Script

Usage: $(basename "$0") [OPTION]

Options:
  build         Build kernel module and user application
  clean         Clean build artifacts
  driver        Build only kernel module
  app           Build only user application
  load          Load kernel module (requires root)
  unload        Unload kernel module (requires root)
  install       Install user application (requires root)
  test          Run unit tests/examples
  help          Display this help message

Examples:
  $(basename "$0") build      # Build everything
  $(basename "$0") driver     # Build kernel module only
  $(basename "$0") app        # Build user application only
  $(basename "$0") load       # Load the kernel module
  $(basename "$0") install    # Install user application

EOF
}

# Function to check if kernel headers are installed
check_kernel_headers() {
    if [ ! -d "/lib/modules/$(uname -r)/build" ]; then
        print_error "Kernel headers not found!"
        print_info "Install kernel headers with:"
        echo "  Debian/Ubuntu: sudo apt-get install linux-headers-\$(uname -r)"
        echo "  Fedora/RHEL:   sudo dnf install kernel-devel"
        exit 1
    fi
}

# Function to build kernel module
build_driver() {
    print_info "Building kernel module..."
    
    cd "$DRIVER_DIR"
    
    if ! command_exists make; then
        print_error "make command not found. Please install build-essential/gcc."
        exit 1
    fi
    
    make clean
    make
    
    if [ -f "$DRIVER_DIR/gpio_device_driver.ko" ]; then
        print_info "Kernel module built successfully: gpio_device_driver.ko"
    else
        print_error "Failed to build kernel module"
        exit 1
    fi
    
    cd - > /dev/null
}

# Function to build user application
build_app() {
    print_info "Building user application..."
    
    cd "$APP_DIR"
    
    if ! command_exists gcc; then
        print_error "gcc compiler not found. Please install gcc."
        exit 1
    fi
    
    make clean
    make
    
    if [ -f "$APP_DIR/gpio_app" ]; then
        print_info "User application built successfully: gpio_app"
    else
        print_error "Failed to build user application"
        exit 1
    fi
    
    cd - > /dev/null
}

# Function to build everything
build_all() {
    print_info "Starting build process..."
    
    check_kernel_headers
    
    build_driver
    build_app
    
    # Copy to build directory
    mkdir -p "$BUILD_DIR"
    cp "$DRIVER_DIR/gpio_device_driver.ko" "$BUILD_DIR/" 2>/dev/null || true
    cp "$APP_DIR/gpio_app" "$BUILD_DIR/" 2>/dev/null || true
    
    print_info "Build complete!"
    print_info "Output files:"
    echo "  Kernel module: $DRIVER_DIR/gpio_device_driver.ko"
    echo "  User app:      $APP_DIR/gpio_app"
}

# Function to clean build artifacts
clean_all() {
    print_info "Cleaning build artifacts..."
    
    cd "$DRIVER_DIR"
    make clean 2>/dev/null || true
    cd - > /dev/null
    
    cd "$APP_DIR"
    make clean 2>/dev/null || true
    cd - > /dev/null
    
    rm -rf "$BUILD_DIR"
    print_info "Clean complete"
}

# Function to load kernel module
load_module() {
    if [ "$(id -u)" != "0" ]; then
        print_error "This operation requires root privileges"
        echo "Run: sudo $0 load"
        exit 1
    fi
    
    if [ ! -f "$DRIVER_DIR/gpio_device_driver.ko" ]; then
        print_error "Kernel module not found. Build it first with: $0 driver"
        exit 1
    fi
    
    print_info "Loading kernel module..."
    insmod "$DRIVER_DIR/gpio_device_driver.ko"
    
    if lsmod | grep -q gpio_device_driver; then
        print_info "Kernel module loaded successfully"
        print_info "Device created: /dev/gpio_dev"
        ls -l /dev/gpio_dev 2>/dev/null || echo "Device node pending udev..."
    else
        print_error "Failed to load kernel module"
        dmesg | tail -5
        exit 1
    fi
}

# Function to unload kernel module
unload_module() {
    if [ "$(id -u)" != "0" ]; then
        print_error "This operation requires root privileges"
        echo "Run: sudo $0 unload"
        exit 1
    fi
    
    if ! lsmod | grep -q gpio_device_driver; then
        print_warn "Kernel module is not loaded"
        return 0
    fi
    
    print_info "Unloading kernel module..."
    rmmod gpio_device_driver
    
    if ! lsmod | grep -q gpio_device_driver; then
        print_info "Kernel module unloaded successfully"
    else
        print_error "Failed to unload kernel module"
        exit 1
    fi
}

# Function to install user application
install_app() {
    if [ "$(id -u)" != "0" ]; then
        print_error "This operation requires root privileges"
        echo "Run: sudo $0 install"
        exit 1
    fi
    
    if [ ! -f "$APP_DIR/gpio_app" ]; then
        print_error "User application not found. Build it first with: $0 app"
        exit 1
    fi
    
    print_info "Installing user application..."
    mkdir -p /usr/local/bin
    install -m 755 "$APP_DIR/gpio_app" /usr/local/bin/
    
    print_info "User application installed to /usr/local/bin/gpio_app"
}

# Function to run tests/examples
run_tests() {
    if [ ! -f "$APP_DIR/gpio_app" ]; then
        print_error "User application not found. Build it first with: $0 app"
        exit 1
    fi
    
    if [ ! -c /dev/gpio_dev ]; then
        print_error "GPIO device not found at /dev/gpio_dev"
        print_info "Load the kernel module first: sudo $0 load"
        exit 1
    fi
    
    print_info "Running GPIO tests/examples..."
    
    cd "$APP_DIR"
    make example
    cd - > /dev/null
}

# Main script logic
case "${1:-help}" in
    build)
        build_all
        ;;
    clean)
        clean_all
        ;;
    driver)
        check_kernel_headers
        build_driver
        ;;
    app)
        build_app
        ;;
    load)
        load_module
        ;;
    unload)
        unload_module
        ;;
    install)
        install_app
        ;;
    test)
        run_tests
        ;;
    help|--help|-h)
        usage
        ;;
    *)
        print_error "Unknown option: $1"
        usage
        exit 1
        ;;
esac
