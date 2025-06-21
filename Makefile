# Target executable name
TARGET = build/doctor_scheduler_gui

# Compiler
CC = gcc

# Source files
SRCS = src/main.c src/doctor_list.c src/scheduler.c src/csv_utils.c

# Object files
OBJS = $(patsubst src/%.c,build/%.o,$(SRCS))

# Include paths
INCLUDES = -Iinclude -Isrc

# Compiler flags
CFLAGS = -Wall -g $(shell pkg-config --cflags gtk+-3.0) $(INCLUDES) -fPIC

# Linker flags
LDFLAGS = $(shell pkg-config --libs gtk+-3.0) -no-pie

# Default target
all: build_dir $(TARGET)

# Create build directory
build_dir:
	mkdir -p build

# Link executable
$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(LDFLAGS)

# Compile source files
build/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean build artifacts
clean:
	rm -rf build

# Install dependencies
deps:
	sudo apt-get install -y build-essential pkg-config libgtk-3-dev

.PHONY: all clean build_dir deps
