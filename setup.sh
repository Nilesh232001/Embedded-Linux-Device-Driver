#!/bin/bash
# GPIO Device Driver Setup Script
# Automatic setup, build, and deployment

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# Colors
GREEN='\033[0;32m'
BLUE='\033[0;34m'
NC='\033[0m'

echo -e "${BLUE}"
echo "╔════════════════════════════════════════════╗"
echo "║   GPIO Device Driver - Quick Setup         ║"
echo "╚════════════════════════════════════════════╝"
echo -e "${NC}"

# Make build script executable
chmod +x "$SCRIPT_DIR/build.sh"

# Check system
echo "Checking system requirements..."

# Check for make
if ! command -v make &> /dev/null; then
    echo "Installing build-essential..."
    sudo apt-get update
    sudo apt-get install -y build-essential
fi

# Check for kernel headers
if [ ! -d "/lib/modules/$(uname -r)/build" ]; then
    echo "Installing kernel headers..."
    sudo apt-get install -y linux-headers-$(uname -r)
fi

# Check for device tree compiler
if ! command -v dtc &> /dev/null; then
    echo "Installing device-tree-compiler..."
    sudo apt-get install -y device-tree-compiler
fi

echo -e "${GREEN}✓ System check complete${NC}"

# Build the project
echo ""
echo "Building project..."
"$SCRIPT_DIR/build.sh" build

# Display next steps
echo ""
echo -e "${BLUE}════════════════════════════════════════${NC}"
echo -e "${GREEN}Build Complete!${NC}"
echo -e "${BLUE}════════════════════════════════════════${NC}"
echo ""
echo "Next steps:"
echo "1. Load the kernel module:"
echo "   sudo $SCRIPT_DIR/build.sh load"
echo ""
echo "2. Run the user application:"
echo "   $SCRIPT_DIR/user_app/gpio_app read"
echo ""
echo "3. For interactive mode:"
echo "   $SCRIPT_DIR/user_app/gpio_app interactive"
echo ""
echo "4. Monitor kernel messages:"
echo "   dmesg -w | grep GPIO_DRIVER"
echo ""
