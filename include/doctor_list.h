
#ifndef DOCTOR_LIST_H
#define DOCTOR_LIST_H

// ----- INCLUDES -----
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ----- KONSTANTA GLOBAL -----
#define MAX_NAME_LENGTH 50  // Panjang maksimum nama dokter
#define NUM_WEEKS 5         // Total minggu dalam satu periode jadwal
#define NUM_DAYS_PER_WEEK 7 // Jumlah hari dalam seminggu
#define NUM_DAYS 30         // Total hari dalam satu periode jadwal
#define NUM_SHIFTS_PER_DAY                                                     \
    3 // Jumlah shift per hari (0: Pagi, 1: Siang, 2: Malam)

/**
 * @struct Doctor
 * @brief  Struktur data untuk menyimpan semua informasi tentang seorang dokter.
 *
 * @var Doctor::id ID unik untuk setiap dokter.
 * @var Doctor::name Nama lengkap dokter.
 * @var Doctor::max_shifts_per_week Jumlah maksimal shift yang bisa diambil
 * dokter dalam seminggu.
 * @var Doctor::shifts_scheduled_per_week Array untuk melacak jumlah shift yang
 * sudah dijadwalkan per minggu.
 * @var Doctor::preference Matriks 2D (7x3) untuk menyimpan preferensi
 * ketersediaan dokter. 1 = bersedia, 0 = tidak.
 * @var Doctor::next Pointer ke node dokter selanjutnya dalam linked list.
 */
typedef struct Doctor {
        int id;
        char name[MAX_NAME_LENGTH];
        int max_shifts_per_week;
        int shifts_scheduled_per_week[NUM_WEEKS];
        int preference[NUM_DAYS_PER_WEEK][NUM_SHIFTS_PER_DAY];
        struct Doctor *next;
} Doctor;

// ----- DEKLARASI VARIABEL GLOBAL -----
// Variabel eksternal ini didefinisikan di doctor_list.c
extern Doctor *head; // Pointer ke kepala (awal) dari linked list dokter.
extern int schedule[NUM_DAYS]
                   [NUM_SHIFTS_PER_DAY]; // Array 2D [30][3] untuk menyimpan ID
                                         // dokter di setiap slot jadwal.
extern int next_doctor_id; // Counter untuk memberikan ID unik ke dokter baru.

// ----- PROTOTYPE FUNGSI -----

/**
 * @brief Menginisialisasi array jadwal.
 * @details Mengatur semua slot jadwal ke 0, yang artinya "kosong" atau "belum
 * ada dokter".
 */
void initialize_schedule();

/**
 * @brief Menambahkan dokter baru ke dalam linked list.
 * @param name Nama dokter baru.
 * @param max_shifts_per_week Jumlah maksimal shift per minggu untuk dokter ini.
 */
void addDoctor(const char *name, int max_shifts_per_week);

/**
 * @brief Menghapus dokter dari daftar berdasarkan ID-nya.
 * @details Fungsi ini juga akan menghapus dokter tersebut dari semua jadwal
 * yang ada dan memperbarui ID dokter-dokter lain agar tetap urut.
 * @param id ID dokter yang akan dihapus.
 */
void removeDoctor(int id);

/**
 * @brief Menampilkan semua dokter yang ada di dalam daftar beserta detail
 * informasinya.
 */
void displayDoctors();

/**
 * @brief Mengatur preferensi ketersediaan seorang dokter untuk shift tertentu.
 * @param doctor_id ID dokter yang preferensinya akan diubah.
 * @param day Hari dalam seminggu (0-6, Senin-Minggu).
 * @param shift Jenis shift (0-2, Pagi-Malam).
 * @param can_do Status ketersediaan (1 jika bisa, 0 jika tidak).
 */
void setDoctorPreference(int doctor_id, int day, int shift, int can_do);

/**
 * @brief Membebaskan semua memori yang dialokasikan untuk linked list dokter.
 * @details Penting untuk dipanggil sebelum program berakhir untuk menghindari
 * memory leak.
 */
void freeDoctorList();

/**
 * @brief Memperbarui ID dokter secara berurutan dan menyesuaikan jadwal yang
 * ada.
 * @details Jika seorang dokter dihapus, ID dokter setelahnya akan digeser maju
 * agar tidak ada "lubang" dalam urutan ID. Jadwal yang ada juga
 * diperbarui untuk mencerminkan perubahan ID ini.
 */
void refreshDoctorIDandSchedule();

/**
 * @brief Mencari dokter dalam daftar berdasarkan ID uniknya.
 * @param id ID dokter yang dicari.
 * @return Pointer ke struct Doctor jika ditemukan, atau NULL jika tidak.
 */
Doctor *findDoctorById(int id);

/**
 * @brief Mencari dokter dalam daftar berdasarkan namanya.
 * @param name Nama dokter yang dicari.
 * @return Pointer ke struct Doctor jika ditemukan, atau NULL jika tidak.
 */
Doctor *findDoctorByName(const char *name);

/**
 * @brief Memperbarui informasi dokter (nama dan/atau maks shift) berdasarkan
 * ID. (Tidak termasuk Preferensi)
 * @param id ID dokter yang akan diperbarui.
 * @param new_name Nama baru untuk dokter.
 * @param new_max_shifts Jumlah maksimal shift per minggu yang baru.
 */
void updateDoctor(int id, const char *new_name, int new_max_shifts);

#endif // DOCTOR_LIST_H
