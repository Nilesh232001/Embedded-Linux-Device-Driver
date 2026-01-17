# Test Script for GPIO Device Driver
# Comprehensive testing of driver functionality

#!/bin/bash

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

# Test counters
TESTS_TOTAL=0
TESTS_PASSED=0
TESTS_FAILED=0

# Utility functions
test_start() {
    echo -e "${BLUE}[TEST $((TESTS_TOTAL+1))]${NC} $1"
    ((TESTS_TOTAL++))
}

test_pass() {
    echo -e "${GREEN}✓ PASS${NC}: $1"
    ((TESTS_PASSED++))
}

test_fail() {
    echo -e "${RED}✗ FAIL${NC}: $1"
    ((TESTS_FAILED++))
}

test_warn() {
    echo -e "${YELLOW}⚠ WARN${NC}: $1"
}

print_summary() {
    echo ""
    echo "╔════════════════════════════════════════╗"
    echo "║         Test Summary                   ║"
    echo "╠════════════════════════════════════════╣"
    echo "║ Total:  $TESTS_TOTAL"
    echo "║ Passed: ${GREEN}$TESTS_PASSED${NC}"
    echo "║ Failed: ${RED}$TESTS_FAILED${NC}"
    echo "╚════════════════════════════════════════╝"
}

# Check if running as root
check_root() {
    if [ "$(id -u)" != "0" ]; then
        echo -e "${YELLOW}Warning: Some tests require root privileges${NC}"
        echo "Run with: sudo $0"
    fi
}

# Test 1: Check GPIO device file
test_device_exists() {
    test_start "Check GPIO device file exists"
    
    if [ -c /dev/gpio_dev ]; then
        test_pass "Device file /dev/gpio_dev exists"
    else
        test_fail "Device file /dev/gpio_dev not found"
        test_warn "Try loading module: sudo make -C driver load"
    fi
}

# Test 2: Check device permissions
test_device_permissions() {
    test_start "Check device file permissions"
    
    PERMS=$(stat -c %a /dev/gpio_dev 2>/dev/null)
    if [ ! -z "$PERMS" ]; then
        echo "Device permissions: $PERMS"
        test_pass "Device file permissions readable"
    else
        test_fail "Cannot read device permissions"
    fi
}

# Test 3: Check kernel module loaded
test_module_loaded() {
    test_start "Check kernel module is loaded"
    
    if lsmod | grep -q gpio_device_driver; then
        test_pass "Kernel module gpio_device_driver is loaded"
        
        # Show module info
        SIZE=$(lsmod | grep gpio_device_driver | awk '{print $2}')
        echo "  Module size: $SIZE bytes"
    else
        test_fail "Kernel module gpio_device_driver not loaded"
        test_warn "Try loading: sudo make -C driver load"
    fi
}

# Test 4: Check kernel messages
test_kernel_messages() {
    test_start "Check kernel messages"
    
    if dmesg | grep -q GPIO_DRIVER; then
        MSG_COUNT=$(dmesg | grep GPIO_DRIVER | wc -l)
        test_pass "Found $MSG_COUNT GPIO_DRIVER messages in kernel log"
        
        # Show last message
        echo "Last message:"
        dmesg | grep GPIO_DRIVER | tail -1 | sed 's/^/  /'
    else
        test_fail "No GPIO_DRIVER messages in kernel log"
    fi
}

# Test 5: Check user application
test_app_executable() {
    test_start "Check user application executable"
    
    APP_PATH="../user_app/gpio_app"
    if [ -x "$APP_PATH" ]; then
        test_pass "User application gpio_app is executable"
        
        # Show version/help
        VERSION=$("$APP_PATH" help 2>&1 | head -1)
        echo "  $VERSION"
    else
        test_fail "User application gpio_app not found or not executable"
        test_warn "Build with: make -C user_app"
    fi
}

# Test 6: Test read operation
test_read_operation() {
    test_start "Test read operation"
    
    if [ ! -c /dev/gpio_dev ]; then
        test_warn "Skipped - device not available"
        return
    fi
    
    APP_PATH="../user_app/gpio_app"
    if [ ! -x "$APP_PATH" ]; then
        test_warn "Skipped - application not available"
        return
    fi
    
    OUTPUT=$("$APP_PATH" read 2>&1)
    if echo "$OUTPUT" | grep -q "SUCCESS"; then
        test_pass "Read operation successful"
        echo "  Output: $(echo $OUTPUT | head -1)"
    else
        test_fail "Read operation failed"
        echo "  Output: $OUTPUT"
    fi
}

# Test 7: Test write operation
test_write_operation() {
    test_start "Test write operation"
    
    if [ ! -c /dev/gpio_dev ]; then
        test_warn "Skipped - device not available"
        return
    fi
    
    APP_PATH="../user_app/gpio_app"
    if [ ! -x "$APP_PATH" ]; then
        test_warn "Skipped - application not available"
        return
    fi
    
    OUTPUT=$("$APP_PATH" write 1 2>&1)
    if echo "$OUTPUT" | grep -q "SUCCESS"; then
        test_pass "Write operation successful"
        echo "  Written value: 1"
    else
        test_fail "Write operation failed"
        echo "  Output: $OUTPUT"
    fi
}

# Test 8: Test status command
test_status_command() {
    test_start "Test status command"
    
    if [ ! -c /dev/gpio_dev ]; then
        test_warn "Skipped - device not available"
        return
    fi
    
    APP_PATH="../user_app/gpio_app"
    if [ ! -x "$APP_PATH" ]; then
        test_warn "Skipped - application not available"
        return
    fi
    
    OUTPUT=$("$APP_PATH" status 2>&1)
    if echo "$OUTPUT" | grep -q "GPIO Status"; then
        test_pass "Status command successful"
    else
        test_fail "Status command failed"
    fi
}

# Test 9: Check device tree compatibility
test_device_tree() {
    test_start "Check Device Tree configuration"
    
    if [ -d "/proc/device-tree" ]; then
        if find /proc/device-tree -name "*gpio*" 2>/dev/null | grep -q .; then
            test_pass "GPIO device tree entries found"
        else
            test_warn "No GPIO device tree entries found"
        fi
    else
        test_warn "Device Tree not available on this system"
    fi
}

# Test 10: Check module parameters
test_module_params() {
    test_start "Check module parameters"
    
    PARAMS_FILE="/sys/module/gpio_device_driver/parameters"
    if [ -d "$PARAMS_FILE" ]; then
        test_pass "Module parameters directory exists"
        
        echo "Available parameters:"
        ls -1 "$PARAMS_FILE" | sed 's/^/  /'
    else
        test_warn "Module parameters not available"
    fi
}

# Main execution
main() {
    echo -e "${BLUE}"
    echo "╔════════════════════════════════════════╗"
    echo "║   GPIO Device Driver - Test Suite      ║"
    echo "╚════════════════════════════════════════╝"
    echo -e "${NC}"
    
    check_root
    
    echo ""
    echo "Running tests..."
    echo "==============="
    echo ""
    
    test_device_exists
    echo ""
    
    test_device_permissions
    echo ""
    
    test_module_loaded
    echo ""
    
    test_kernel_messages
    echo ""
    
    test_app_executable
    echo ""
    
    test_read_operation
    echo ""
    
    test_write_operation
    echo ""
    
    test_status_command
    echo ""
    
    test_device_tree
    echo ""
    
    test_module_params
    echo ""
    
    print_summary
    
    # Return appropriate exit code
    [ $TESTS_FAILED -eq 0 ] && return 0 || return 1
}

# Run main function
main
