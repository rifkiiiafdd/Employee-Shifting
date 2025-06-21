# Nama file executable yang akan dihasilkan
TARGET = doctor_scheduler_gui

# Compiler yang digunakan
CC = gcc

# File-file source code .c yang akan dikompilasi
# PERHATIKAN: main.c (versi CLI) tidak disertakan untuk menghindari konflik
SRCS = gui_main.c doctor_list.c scheduler.c csv_utils.c

# Objek file yang akan dibuat dari source
OBJS = $(SRCS:.c=.o)

# Flags untuk compiler C
# pkg-config akan otomatis menambahkan flag yang dibutuhkan untuk GTK 3
CFLAGS = -Wall -g $(shell pkg-config --cflags gtk+-3.0)

# Flags untuk linker
# pkg-config akan otomatis menambahkan library yang dibutuhkan untuk GTK 3
LDFLAGS = $(shell pkg-config --libs gtk+-3.0)

# Aturan default: kompilasi semuanya
all: $(TARGET)

# Aturan untuk membuat file executable
$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(LDFLAGS)

# Aturan untuk mengompilasi file .c menjadi .o
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Aturan untuk membersihkan file hasil kompilasi
clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean