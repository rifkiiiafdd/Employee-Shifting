#include "view_schedule.h"

/**
 * @brief Mencari angka random pada rentang 0 sampai jumlah doktor yang ada
 * @param number_doctor jumlah dokter yang terdata
 * @return angka random dalam rentang jumlah dokter
 */
int rng(int number_doctor);

/**
 * @brief Menambahkan data dokter ke Array of struct
 * @param id id dokter
 * @param name nama dokter
 * @param max_shift maksimal shift dokter dalam seminggu
 * @param preference data preferensi shift dokter
 */
void add(int id, char *name, int max_shift, int preference[7][3]);

/**
 * @brief mengecek preferensi dokter
 * @param id id dokter
 * @param day hari-ke..
 * @param shift shift ke ..
 * @return 1 jika dokter tidak mau berjaga pada shift tersebut
 */
int check_preference(int id, int day, int shift);

/**
 * @brief menyusun jadwal dokter se-optimal mungkin mempertimbangkan shift
 * maksimal dokter dan preferensinya
 * @param schedule matriks 30*3 yang menyimpan data jadwal selama satu bulan
 * @param succes parameter yang memberi tahu apakah dokter yang ada cukup
 */
void generate_schedule(int schedule[30][3]);

/**
 * @brief Menambahkan data jadwal yang tabrakan ke linked list tabrakan
 * @param hari tanggal saat tabrakan
 * @param shift shift yang tabrakan
 * @param id id dokter yang tabrakan
 * @param name nama dokter yang tabrakan**/
void conflict_schedule(int hari, int shift, int id, char *name);

/**
 * @brief mencetak data matriks
 * @param baris jumlah baris
 * @param kolom jumlah kolom
 * @param matriks matriks yang dicetak
 */
void printing_matriks(int baris, int kolom, int matriks[baris][kolom]);

/**
 * @brief menghapus dokter berdasarkan id
 * @param data data dokter
 * @param number_doctor jumlah dokter
 * @param id_targer id dari dokter yang ingin di hapus
 */
void remove_doctor(Doctor **data, int *number_doctor, int id_target);
