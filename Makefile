
# Makefile for Doctor Scheduler Application

# Define the C compiler to use
CC = gcc

# Use pkg-config to get GTK3 compiler flags and linker libraries
GTK_CFLAGS = $(shell pkg-config --cflags gtk+-3.0)
GTK_LIBS = $(shell pkg-config --libs gtk+-3.0)

# Define compiler flags
# -Wall: Enables all common warnings
# -g: Includes debugging information (for debugging with gdb)
# -std=c11: Specifies the C11 standard
# -Iinclude: IMPORTANT - Tells the compiler to look for headers in the 'include' directory
CFLAGS = -Wall -g -std=c11 -Iinclude $(GTK_CFLAGS)

# Define the name of the executable, now in the build directory
TARGET = build/doctor_scheduler

# Define the source files, now located in the src directory
SRCS = src/main_program.c src/doctor_list.c src/csv_utils.c src/gui.c

# Automatically generate object file names in the build/ directory
OBJS = $(patsubst src/%.c,build/%.o,$(SRCS))

# Default target: builds the executable
all: build $(TARGET)

# Create the build directory if it doesn't exist
build:
	mkdir -p build

# Rule to link object files into the executable
$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(GTK_LIBS)

# Rule to compile each .c file into a .o object file in the build/ directory
# $<: The name of the prerequisite (e.g., src/main_program.c)
# $@: The name of the target (e.g., build/main_program.o)
build/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Phony targets are not actual files, but commands
.PHONY: all clean run build

# Clean target: removes compiled files from build/
clean:
	rm -rf build/*.o build/doctor_scheduler

# Run target: executes the compiled program from the build directory
run: $(TARGET)
	./$(TARGET)


