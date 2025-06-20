/*
 * view_schedule.h
 * Header file untuk modul tampilan jadwal jaga dokter
 * 
 * Menyediakan fungsi-fungsi untuk menampilkan jadwal dalam berbagai format
 * dan memberikan informasi statistik serta pelanggaran preferensi
 */

#ifndef VIEW_SCHEDULE_H
#define VIEW_SCHEDULE_H

// Struktur data yang digunakan (sesuai dengan implementasi yang ada)
typedef struct Doctor {
    int id;
    char name[20];
    int max_shift_per_week;
    int shift_scheduled_per_week[5];
    int preference[7][3];
} Doctor;

typedef struct conflict {
    int id;
    char name[20];
    int hari;
    int shift;
    struct conflict* next;
} conflict;

// Fungsi-fungsi untuk tampilan jadwal

/**
 * @brief Menampilkan jadwal jaga harian untuk hari tertentu
 * @param day Hari ke- (0-29)
 * @param schedule Matrix jadwal [30][3]
 * @param data Array pointer ke data dokter
 * @param number_doctor Jumlah dokter
 */
void displayDailySchedule(int day, int schedule[30][3], Doctor** data, int number_doctor);

/**
 * @brief Menampilkan jadwal jaga mingguan untuk minggu tertentu
 * @param week Minggu ke- (0-4)
 * @param schedule Matrix jadwal [30][3]
 * @param data Array pointer ke data dokter
 * @param number_doctor Jumlah dokter
 */
void displayWeeklySchedule(int week, int schedule[30][3], Doctor** data, int number_doctor);

/**
 * @brief Menampilkan jadwal jaga bulanan (30 hari penuh)
 * @param schedule Matrix jadwal [30][3]
 * @param data Array pointer ke data dokter
 * @param number_doctor Jumlah dokter
 */
void displayMonthlySchedule(int schedule[30][3], Doctor** data, int number_doctor);

/**
 * @brief Menampilkan statistik shift per dokter
 * @param schedule Matrix jadwal [30][3]
 * @param data Array pointer ke data dokter
 * @param number_doctor Jumlah dokter
 */
void displayShiftStatistics(int schedule[30][3], Doctor** data, int number_doctor);

/**
 * @brief Menampilkan informasi pelanggaran preferensi dokter
 * @param head Pointer ke head dari linked list conflict
 */
void displayPreferenceViolations(conflict* head);

/**
 * @brief Menu interaktif untuk memilih jenis tampilan jadwal
 * @param schedule Matrix jadwal [30][3]
 * @param data Array pointer ke data dokter
 * @param number_doctor Jumlah dokter
 * @param conflict_head Pointer ke head dari linked list conflict
 */
void scheduleViewMenu(int schedule[30][3], Doctor** data, int number_doctor, conflict* conflict_head);

// Fungsi utilitas

/**
 * @brief Mendapatkan nama dokter berdasarkan ID
 * @param id ID dokter
 * @param data Array pointer ke data dokter
 * @param number_doctor Jumlah dokter
 * @return Pointer ke string nama dokter
 */
const char* getDoctorNameById(int id, Doctor** data, int number_doctor);

/**
 * @brief Mencetak garis pemisah untuk formatting
 * @param length Panjang garis
 */
void printSeparator(int length);

/**
 * @brief Mencetak header jadwal dengan format yang rapi
 * @param title Judul header
 */
void printScheduleHeader(const char* title);

#endif // VIEW_SCHEDULE_H