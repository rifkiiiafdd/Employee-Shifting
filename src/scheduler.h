#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "doctor_list.h"

// Struktur untuk menyimpan data konflik
typedef struct Conflict {
        int id;
        char name[MAX_NAME_LENGTH];
        int hari;
        int shift;
        struct Conflict *next;
} Conflict;

extern Conflict *chead;

// --- Fungsi Penjadwalan Utama ---
// Menghasilkan jadwal dokter untuk 30 hari berdasarkan preferensi dan aturan.
void generate_schedule();
// Menampilkan daftar konflik jadwal yang tidak dapat diselesaikan.
void view_conflict();
// Membebaskan memori dari daftar konflik.
void free_conflict_list();

// --- Fungsi Pengambil Jadwal (Model Data untuk Tampilan) ---

// --- Untuk Dokter Spesifik (Tugas Akhyar) ---
// Mengambil shift yang dijadwalkan untuk dokter tertentu pada hari & minggu
// spesifik.
int getDoctorScheduleDay(int week, int day, int doctorId, int outShifts[]);
// Mengambil jadwal lengkap seorang dokter untuk satu minggu penuh.
void getDoctorScheduleWeek(int week, int doctorId,
                           int outShifts[NUM_DAYS_PER_WEEK][NUM_SHIFTS_PER_DAY],
                           int outCount[NUM_DAYS_PER_WEEK]);
// Mengambil jadwal lengkap seorang dokter untuk satu bulan penuh (5 minggu).
void get_doctor_schedule_month(
    int doctor_id,
    int out_shifts[NUM_WEEKS][NUM_DAYS_PER_WEEK][NUM_SHIFTS_PER_DAY],
    int out_count[NUM_WEEKS][NUM_DAYS_PER_WEEK]);

// --- Untuk Semua Dokter (Tugas Vyto - Model Baru) ---
// Mengambil ID dokter untuk setiap shift pada hari tertentu.
void get_all_doctors_schedule_day(int week, int day,
                                  int out_doctor_ids[NUM_SHIFTS_PER_DAY]);
// Mengambil ID dokter untuk setiap shift dalam satu minggu penuh.
void get_all_doctors_schedule_week(
    int week, int out_doctor_ids[NUM_DAYS_PER_WEEK][NUM_SHIFTS_PER_DAY]);
// Mengambil ID dokter untuk seluruh jadwal 30 hari.
void get_all_doctors_schedule_month(
    int out_doctor_ids[NUM_DAYS][NUM_SHIFTS_PER_DAY]);

// --- Fungsi Tampilan Jadwal (Model Lama - untuk referensi) ---
void getScheduleDay(int mingguKe, int hariKe);
void getScheduleWeek(int mingguKe);

// Menjumlahkan total shift untuk dokter tertentu untuk satu hari spesifik
int totalShiftDay(int week, int day, int id);

// Menjumlahkan total shift untuk dokter tertentu untuk satu minggu spesifik
int totalShiftWeek(int week, int id);

// Menjumlahkan total shift untuk dokter tertentu untuk 30 hari
int totalShift(int id);

#endif // SCHEDULER_H
