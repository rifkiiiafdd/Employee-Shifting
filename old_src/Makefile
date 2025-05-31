
# Makefile for Doctor Scheduler Application

# Define the C compiler to use
CC = gcc

# Define compiler flags
# -Wall: Enables all common warnings
# -g: Includes debugging information
# -std=c11: Specifies the C11 standard
CFLAGS = -Wall -g -std=c11

# Define the name of the executable
TARGET = doctor_scheduler

# Define the source files
SRCS = main.c doctor_list.c csv_utils.c

# Automatically generate object file names from source files
OBJS = $(SRCS:.c=.o)

# Default target: builds the executable
all: $(TARGET)

# Rule to link object files into the executable
$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET)

# Rule to compile each .c file into a .o object file
# $<: The name of the prerequisite (the .c file)
# $@: The name of the target (the .o file)
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Phony targets are not actual files, but commands
.PHONY: all clean run

# Clean target: removes compiled files
clean:
	rm -f $(OBJS) $(TARGET)

# Run target: executes the compiled program
run: $(TARGET)
	./$(TARGET)

