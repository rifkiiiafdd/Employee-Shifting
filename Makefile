
# Makefile for Doctor Scheduler CLI
#
# Automatically finds source files and works on Linux, macOS, and Windows.

# --- Compiler ---
CC = gcc

# --- Platform Detection ---
# Default to Unix-like OS and override for Windows (using MinGW/MSYS2)
detected_OS := $(shell uname -s)
ifeq ($(OS),Windows_NT)
	detected_OS := Windows
endif

# --- Project Structure ---
TARGET_NAME = doctor_scheduler
SRC_DIR = src
INC_DIR = include
BUILD_DIR = build

# --- Automatic File Discovery ---
# Find all .c files in the source directory. This is the "extensible" part.
# Any new .c file added to `src/` will be automatically included.
SRCS = $(wildcard $(SRC_DIR)/*.c)

# Generate object file names, placing them in the build directory.
OBJS = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRCS))

# --- Flags ---
# CFLAGS: Compiler flags.
# -I$(INC_DIR) tells the compiler to look for headers in the 'include' directory.
# -Wall enables all common warnings. -g adds debug info.
CFLAGS = -Wall -g -I$(INC_DIR)

# LDFLAGS: Linker flags (none needed for this simple project).
LDFLAGS =

# --- Platform-Specific Variables ---
ifeq ($(detected_OS),Windows)
	# Windows-specific settings
	TARGET = $(BUILD_DIR)/$(TARGET_NAME).exe
	RM = rmdir /s /q
	MKDIR_P = if not exist $(subst /,\,$(BUILD_DIR)) mkdir $(subst /,\,$(BUILD_DIR))
	RUN_CMD = $(subst /,\,$(TARGET))
else
	# Linux, macOS, and other Unix-like OS settings
	TARGET = $(BUILD_DIR)/$(TARGET_NAME)
	RM = rm -rf
	MKDIR_P = mkdir -p
	RUN_CMD = ./$(TARGET)
endif

# --- Main Targets ---

# Default target: build the program.
# The .DEFAULT_GOAL makes `make` run this target if none is specified.
.DEFAULT_GOAL := all
all: $(TARGET)

# Rule to link the object files into the final executable.
# '$^' is an automatic variable for all prerequisites (the object files).
# '$@' is an automatic variable for the target name (the executable).
$(TARGET): $(OBJS)
	@echo "==> Linking target: $@"
	$(CC) $^ -o $@ $(LDFLAGS)
	@echo "==> Build complete! Run with 'make run'."

# Pattern rule to compile a source file from `src/` into an object file in `build/`.
# This rule is applied for each file in $(OBJS).
# The `| $(BUILD_DIR)` is an order-only prerequisite to ensure the build
# directory exists before compilation starts.
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	@echo "==> Compiling: $<"
	$(CC) $(CFLAGS) -c $< -o $@

# Rule to create the build directory. This runs only once if the directory doesn't exist.
$(BUILD_DIR):
	@echo "==> Creating build directory..."
	@$(MKDIR_P) $(BUILD_DIR)


# --- Command Targets ---

# Phony targets are commands, not files.
.PHONY: all clean run help

# Build the program (alias for 'all').
build: all

# Clean all build artifacts by removing the build directory.
# The hyphen '-' before the command tells make to ignore errors (e.g., if the directory doesn't exist).
clean:
	@echo "==> Cleaning build artifacts..."
	-$(RM) $(BUILD_DIR)
	@echo "==> Clean complete."

# Build (if necessary) and run the program.
run: all
	@echo "==> Running application..."
	@$(RUN_CMD)

# Show a helpful message with available commands.
help:
	@echo "Available commands:"
	@echo "  make build    - Compiles the project (default)."
	@echo "  make run      - Compiles and runs the application."
	@echo "  make clean    - Removes all compiled files."
	@echo "  make help     - Shows this help message."


