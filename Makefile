# ------------------------- #
#   Platform Detection      #
# ------------------------- #

# Detect OS
ifeq ($(OS),Windows_NT)
    PLATFORM := Windows
else
    PLATFORM := $(shell uname -s)
endif

# ------------------------- #
#   Platform-Specific Setup #
# ------------------------- #

ifeq ($(PLATFORM),Windows)
    # Windows settings (MinGW)
    TARGET := bin/doctor_scheduler_gui.exe
    CC := gcc
    PKG_CONFIG := pkg-config
    LDFLAGS += -mwindows  # Prevent console window
    MKDIR := mkdir -p
    RM := rm -rf
else
    # Linux settings
    TARGET := bin/doctor_scheduler_gui
    CC := gcc
    PKG_CONFIG := pkg-config
    MKDIR := mkdir -p
    RM := rm -rf
endif

# ------------------------- #
#   Project Configuration   #
# ------------------------- #

# Source files
SRCS := src/main.c src/doctor_list.c src/scheduler.c src/csv_utils.c

# Objects
OBJS := $(patsubst src/%.c,build/%.o,$(SRCS))

# Directories
BUILD_DIR := build
BIN_DIR := bin

# Compiler flags
CFLAGS := -Wall -g $(shell $(PKG_CONFIG) --cflags gtk+-3.0) -Iinclude -Isrc

# Linker flags
LDFLAGS += $(shell $(PKG_CONFIG) --libs gtk+-3.0)

# ------------------------- #
#   Build Targets           #
# ------------------------- #

all: dirs $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

$(BUILD_DIR)/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

dirs:
	@$(MKDIR) $(BUILD_DIR) $(BIN_DIR)

clean:
	$(RM) $(BUILD_DIR) $(BIN_DIR)

run: all
	./$(TARGET)

# ------------------------- #
#   Platform-Specific Help  #
# ------------------------- #

install-deps:
ifeq ($(PLATFORM),Windows)
	@echo "Install MSYS2 from https://www.msys2.org/"
	@echo "Then run in MinGW64 shell:"
	@echo "pacman -S mingw-w64-x86_64-gtk3 mingw-w64-x86_64-pkg-config"
else
	@echo "Run:"
	@echo "sudo apt install -y build-essential pkg-config libgtk-3-dev"
endif

.PHONY: all dirs clean run install-deps
