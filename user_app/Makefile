# Makefile for User-Space GPIO Application
# Builds an interactive GPIO control application

# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -O2 -I./include
LDFLAGS = -lpthread

# Output executable
TARGET = gpio_app

# Source files
SOURCES = src/main.c src/gpio_control.c
HEADERS = include/gpio_control.h

# Object files (derived from sources)
OBJECTS = $(SOURCES:.c=.o)

# Build targets
all: $(TARGET)

# Build executable
$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
	@echo "Build successful: $(TARGET)"

# Compile each source file to object file
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@
	@echo "Compiled: $<"

# Clean build artifacts
clean:
	rm -f $(OBJECTS) $(TARGET) *~
	@echo "Clean complete"

# Install application (optional)
install: $(TARGET)
	mkdir -p /usr/local/bin
	install -m 755 $(TARGET) /usr/local/bin/
	@echo "Installed $(TARGET) to /usr/local/bin/"

# Run the application
run: $(TARGET)
	./$(TARGET) help

# Run in interactive mode
run-interactive: $(TARGET)
	./$(TARGET) interactive

# Run examples
example: $(TARGET)
	@echo "Running GPIO examples..."
	@echo "1. Read GPIO value"
	./$(TARGET) read
	@echo ""
	@echo "2. Write GPIO value"
	./$(TARGET) write 1
	sleep 1
	./$(TARGET) write 0
	@echo ""
	@echo "3. Blink LED 3 times"
	./$(TARGET) blink 3

# Show usage help
help:
	./$(TARGET) help

# Format code with indent (optional)
format:
	indent -linux -i4 src/*.c include/*.h

# Static analysis (requires cppcheck)
check:
	cppcheck --enable=all src/ include/

# View kernel messages related to GPIO driver
dmesg:
	dmesg | grep -i gpio || echo "No GPIO messages found"

# Monitor kernel messages live
dmesg-live:
	dmesg -w | grep -i gpio

# Dependencies: user_app depends on running kernel driver
# Check if GPIO device exists
verify-driver:
	@if [ -c /dev/gpio_dev ]; then \
		echo "GPIO device found: /dev/gpio_dev"; \
	else \
		echo "ERROR: GPIO device not found!"; \
		echo "Make sure kernel module is loaded: sudo insmod ../driver/gpio_device_driver.ko"; \
		exit 1; \
	fi

# Development build with extra debugging
debug: CFLAGS += -g -DDEBUG
debug: clean all
	@echo "Debug build complete"

# Release build optimized
release: CFLAGS = -Wall -O3 -I./include
release: clean all
	@echo "Release build complete"

# Help message
show-help:
	@echo "GPIO Application - Makefile targets:"
	@echo "  make              - Build the application"
	@echo "  make clean        - Clean build artifacts"
	@echo "  make run          - Run application (show help)"
	@echo "  make run-interactive - Run in interactive mode"
	@echo "  make example      - Run example commands"
	@echo "  make install      - Install to /usr/local/bin (needs sudo)"
	@echo "  make verify-driver - Check if kernel driver is loaded"
	@echo "  make dmesg        - Show GPIO kernel messages"
	@echo "  make dmesg-live   - Monitor kernel messages live"
	@echo "  make debug        - Build with debug symbols"
	@echo "  make release      - Build optimized release"
	@echo "  make format       - Format code (requires indent)"
	@echo "  make check        - Static analysis (requires cppcheck)"
	@echo "  make help         - Show application help"
	@echo "  make show-help    - Show this help message"

.PHONY: all clean run run-interactive example help install verify-driver \
        dmesg dmesg-live debug release format check show-help
