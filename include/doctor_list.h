#ifndef DOCTOR_LIST_H
#define DOCTOR_LIST_H

// Library
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Konstanta
#define MAX_NAME_LENGTH 50
#define NUM_WEEKS 5
#define NUM_DAYS_PER_WEEK 7  // 0-6 (Sun-Sat)
#define NUM_DAYS 30          // Menggunakan 30 hari sesuai permintaan
#define NUM_SHIFTS_PER_DAY 3 // 0: Morning, 1: Afternoon, 2: Night

// Struktur data penyimpanan dokter
typedef struct Doctor {
        int id;
        char name[MAX_NAME_LENGTH];
        int max_shifts_per_week;
        int shifts_scheduled_per_week[NUM_WEEKS];
        int preference[NUM_DAYS_PER_WEEK][NUM_SHIFTS_PER_DAY];
        struct Doctor *next;
} Doctor;

// Deklarasi variable global
extern Doctor *head;
// Menggunakan array 2D [30][3] sesuai permintaan
extern int schedule[NUM_DAYS][NUM_SHIFTS_PER_DAY];
extern int next_doctor_id;

// Deklarasi fungsi (prototype)

// Menginisialisasi jadwal, mengatur semua slot ke 0 (kosong).
void initialize_schedule();

// Menambahkan dokter baru ke dalam linked list.
void addDoctor(const char *name, int max_shifts_per_week);

// Menghapus dokter dari daftar berdasarkan ID.
void removeDoctor(int id);

// Menampilkan semua dokter yang ada di dalam daftar beserta informasinya.
void displayDoctors();

// Mengatur preferensi ketersediaan (bisa/tidak) seorang dokter untuk shift
// tertentu.
void setDoctorPreference(int doctor_id, int day, int shift, int can_do);

// Membebaskan semua memori yang dialokasikan untuk linked list dokter.
void freeDoctorList();

// Memperbarui `next_doctor_id` menjadi satu lebih tinggi dari ID maksimum yang
// ada.
void refreshDoctorID();

// Memperbarui ID dokter
void refreshDoctorIDandSchedule();

// Mencari dokter dalam daftar berdasarkan ID uniknya.
Doctor *findDoctorById(int id);

// Mencari dokter dalam daftar berdasarkan namanya.
Doctor *findDoctorByName(const char *name);

// Memperbarui informasi dokter (nama dan/atau maks shift) berdasarkan ID.
void updateDoctor(int id, const char *new_name, int new_max_shifts);

#endif // DOCTOR_LIST_H
