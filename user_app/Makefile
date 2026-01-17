# Makefile for User-Space GPIO Application

CC = gcc
CFLAGS = -Wall -Wextra -O2 -I./include
LDFLAGS = -lpthread

# Output executable
TARGET = gpio_app

# Source files
SOURCES = src/main.c src/gpio_control.c
OBJECTS = $(SOURCES:.c=.o)

# Default target
all: $(TARGET)

# Build executable
$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Compile object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean build artifacts
clean:
	rm -f $(OBJECTS) $(TARGET)

# Run the application
run: $(TARGET)
	./$(TARGET)

# View kernel messages related to the driver
dmesg:
	dmesg -w | grep gpio_driver

.PHONY: all clean run dmesg
