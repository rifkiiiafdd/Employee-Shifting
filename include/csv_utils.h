#ifndef CSV_UTILS_H
#define CSV_UTILS_H

#include <stdbool.h>

// Deklarasi fungsi (Prototype)

// Menyimpan daftar dokter saat ini (dari linked list) ke sebuah file CSV.
bool save_doctors_to_csv(const char *filepath);

// Memuat daftar dokter dari file CSV, menimpa data dokter yang ada di memori.
bool load_doctors_from_csv(const char *filepath);

// Menyimpan data jadwal (array schedule) ke sebuah file CSV.
bool save_schedule_to_csv(const char *filepath);

// Memuat data jadwal dari file CSV ke dalam array schedule di memori.
bool load_schedule_from_csv(const char *filepath);

#endif // CSV_UTILS_H
