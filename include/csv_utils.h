#ifndef CSV_UTILS_H
#define CSV_UTILS_H

#include <stdbool.h>

/**
 * @brief Menyimpan daftar dokter saat ini (dari linked list) ke sebuah file
 * CSV.
 * @param filepath Path lengkap ke file CSV tujuan (misal: "data/doctors.csv").
 * @return `true` jika penyimpanan berhasil, `false` jika gagal.
 */
bool save_doctors_to_csv(const char *filepath);

/**
 * @brief Memuat daftar dokter dari file CSV, menimpa data dokter yang ada di
 * memori.
 * @details Sebelum memuat, fungsi ini akan menghapus semua data dokter yang ada
 * saat ini.
 * @param filepath Path lengkap ke file CSV sumber (misal: "data/doctors.csv").
 * @return `true` jika pemuatan berhasil, `false` jika file tidak ditemukan atau
 * terjadi error.
 */
bool load_doctors_from_csv(const char *filepath);

/**
 * @brief Menyimpan data jadwal (dari array `schedule` global) ke sebuah file
 * CSV.
 * @param filepath Path lengkap ke file CSV tujuan (misal: "data/schedule.csv").
 * @return `true` jika penyimpanan berhasil, `false` jika gagal.
 */
bool save_schedule_to_csv(const char *filepath);

/**
 * @brief Memuat data jadwal dari file CSV ke dalam array `schedule` di memori.
 * @param filepath Path lengkap ke file CSV sumber (misal: "data/schedule.csv").
 * @return `true` jika pemuatan berhasil, `false` jika file tidak ditemukan atau
 * terjadi error.
 */
bool load_schedule_from_csv(const char *filepath);

#endif // CSV_UTILS_H
