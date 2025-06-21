#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "doctor_list.h"

/**
 * @struct Conflict
 * @brief  Struktur data untuk menyimpan informasi tentang konflik jadwal.
 * @details Konflik terjadi jika seorang dokter dijadwalkan pada shift yang
 * sebenarnya dia tidak bersedia (berdasarkan preferensi).
 *
 * @var Conflict::id ID dokter yang mengalami konflik.
 * @var Conflict::name Nama dokter yang mengalami konflik.
 * @var Conflict::hari Hari ke- (dalam 30 hari) di mana konflik terjadi.
 * @var Conflict::shift Shift ke- (Pagi/Siang/Malam) di mana konflik terjadi.
 * @var Conflict::next Pointer ke node konflik selanjutnya dalam linked list.
 */
typedef struct Conflict {
    int id;
    char name[MAX_NAME_LENGTH];
    int hari;
    int shift;
    struct Conflict *next;
} Conflict;

// Variabel global untuk linked list konflik, didefinisikan di scheduler.c
extern Conflict *chead;

// ----- FUNGSI PENJADWALAN UTAMA -----

/**
 * @brief Menghasilkan jadwal dokter otomatis untuk 30 hari.
 * @details Algoritma ini mencoba mengisi semua slot shift, dengan mempertimbangkan
 * preferensi dokter dan batas maksimal shift per minggu.
 */
void generate_schedule();

/**
 * @brief Menampilkan daftar konflik jadwal yang tidak dapat diselesaikan oleh algoritma.
 */
void view_conflict();

/**
 * @brief Membebaskan memori yang digunakan oleh linked list data konflik.
 */
void free_conflict_list();


// ----- FUNGSI PENGAMBIL DATA JADWAL (GETTER) -----

// --- UNTUK DOKTER SPESIFIK ---

/**
 * @brief Mengambil shift yang dijadwalkan untuk dokter tertentu pada hari & minggu spesifik.
 * @param week Minggu ke- (1-5).
 * @param day Hari ke- dalam minggu (0-6, Senin-Minggu).
 * @param doctorId ID dokter yang dicari.
 * @param outShifts Array output untuk menyimpan daftar shift (0/1/2) yang diisi oleh dokter tersebut.
 * @param outCount Pointer output untuk menyimpan jumlah shift yang ditemukan.
 */
void getDoctorScheduleDay(int week, int day, int doctorId, int outShifts[], int *outCount);

/**
 * @brief Mengambil jadwal lengkap seorang dokter untuk satu minggu penuh.
 * @param week Minggu ke- (1-5).
 * @param doctorId ID dokter yang dicari.
 * @param outShifts Matriks 2D output untuk menyimpan daftar shift per hari.
 * @param outCount Array output untuk menyimpan jumlah shift per hari.
 */
void getDoctorScheduleWeek(int week, int doctorId,
                           int outShifts[NUM_DAYS_PER_WEEK][NUM_SHIFTS_PER_DAY],
                           int outCount[NUM_DAYS_PER_WEEK]);

/**
 * @brief Mengambil jadwal lengkap seorang dokter untuk satu bulan penuh (5 minggu).
 * @param doctor_id ID dokter yang dicari.
 * @param out_shifts Matriks 3D output untuk menyimpan daftar shift per hari per minggu.
 * @param out_count Matriks 2D output untuk menyimpan jumlah shift per hari per minggu.
 */
void get_doctor_schedule_month(
    int doctor_id,
    int out_shifts[NUM_WEEKS][NUM_DAYS_PER_WEEK][NUM_SHIFTS_PER_DAY],
    int out_count[NUM_WEEKS][NUM_DAYS_PER_WEEK]);


// --- UNTUK SEMUA DOKTER ---

/**
 * @brief Mengambil ID dokter untuk setiap shift pada hari dan minggu tertentu.
 * @param week Minggu ke- (1-5).
 * @param day Hari ke- dalam minggu (0-6, Senin-Minggu).
 * @param out_doctor_ids Array output untuk menyimpan ID dokter untuk shift Pagi, Siang, dan Malam.
 */
void get_all_doctors_schedule_day(int week, int day,
                                  int out_doctor_ids[NUM_SHIFTS_PER_DAY]);

/**
 * @brief Mengambil ID dokter untuk setiap shift dalam satu minggu penuh.
 * @param week Minggu ke- (1-5).
 * @param out_doctor_ids Matriks 2D output untuk menyimpan ID dokter untuk semua shift dalam minggu tersebut.
 */
void get_all_doctors_schedule_week(
    int week, int out_doctor_ids[NUM_DAYS_PER_WEEK][NUM_SHIFTS_PER_DAY]);

/**
 * @brief Mengambil ID dokter untuk seluruh jadwal 30 hari.
 * @param out_doctor_ids Matriks 2D output untuk menyimpan seluruh data jadwal.
 */
void get_all_doctors_schedule_month(
    int out_doctor_ids[NUM_DAYS][NUM_SHIFTS_PER_DAY]);


// --- FUNGSI UTILITAS PENGHITUNGAN (MODEL LAMA) ---
// Fungsi-fungsi ini mungkin masih berguna untuk kalkulasi cepat.

/**
 * @brief Menghitung total shift seorang dokter pada hari tertentu.
 * @param week Minggu ke- (1-5).
 * @param day Hari ke- dalam minggu (0-6, Senin-Minggu).
 * @param id ID dokter.
 * @return int Jumlah shift.
 */
int totalShiftDay(int week, int day, int id);

/**
 * @brief Menghitung total shift seorang dokter pada minggu tertentu.
 * @param week Minggu ke- (1-5).
 * @param id ID dokter.
 * @return int Jumlah shift.
 */
int totalShiftWeek(int week, int id);

/**
 * @brief Menghitung total shift seorang dokter selama 30 hari.
 * @param id ID dokter.
 * @return int Jumlah shift.
 */
int totalShift(int id);

#endif // SCHEDULER_H
