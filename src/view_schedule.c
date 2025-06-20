/*
 * view_schedule.c
 * Modul untuk menampilkan jadwal jaga dokter dalam format harian, mingguan, dan
 * bulanan
 *
 * Spesifikasi:
 * - Menampilkan jadwal dengan format yang mudah dibaca
 * - Mendukung tampilan harian, mingguan, dan bulanan
 * - Menampilkan statistik shift per dokter
 * - Menampilkan informasi pelanggaran preferensi
 */

#include "view_schedule.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Array nama hari dalam bahasa Indonesia
static const char *days_name[] = {"Minggu", "Senin", "Selasa", "Rabu",
                                  "Kamis",  "Jumat", "Sabtu"};

// Array nama shift
static const char *shift_name[] = {"Pagi", "Siang", "Malam"};

// Array nama bulan
static const char *month_name[] = {
    "",     "Januari", "Februari",  "Maret",   "April",    "Mei",     "Juni",
    "Juli", "Agustus", "September", "Oktober", "November", "Desember"};

/*
 * Fungsi untuk mendapatkan nama dokter berdasarkan ID
 */
const char *getDoctorNameById(int id, Doctor **data, int number_doctor) {
    if (id <= 0 || id > number_doctor) {
        return "Tidak Ada";
    }
    return data[id - 1]->name;
}

/*
 * Fungsi untuk mencetak garis pemisah
 */
void printSeparator(int length) {
    for (int i = 0; i < length; i++) {
        printf("=");
    }
    printf("\n");
}

/*
 * Fungsi untuk mencetak header jadwal
 */
void printScheduleHeader(const char *title) {
    int title_length = strlen(title);
    int total_length = 80;
    int padding = (total_length - title_length) / 2;

    printSeparator(total_length);
    for (int i = 0; i < padding; i++)
        printf(" ");
    printf("%s\n", title);
    printSeparator(total_length);
}

/*
 * Menampilkan jadwal harian untuk tanggal tertentu
 * Input: day (0-29), schedule matrix, data dokter, jumlah dokter
 */
void displayDailySchedule(int day, int schedule[30][3], Doctor **data,
                          int number_doctor) {
    char title[100];
    sprintf(title, "JADWAL JAGA HARI KE-%d (%s)", day + 1, days_name[day % 7]);

    printScheduleHeader(title);

    printf(
        "\u250c\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500"
        "\u2500\u2500\u2500\u252c\u2500\u2500\u2500\u2500\u2500\u2500\u2500"
        "\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500"
        "\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500"
        "\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500"
        "\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2510\n");
    printf("\u2502    SHIFT    \u2502                   DOKTER JAGA            "
           "        \u2502\n");
    printf(
        "\u251c\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500"
        "\u2500\u2500\u2500\u253c\u2500\u2500\u2500\u2500\u2500\u2500\u2500"
        "\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500"
        "\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500"
        "\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500"
        "\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2524\n");

    for (int shift = 0; shift < 3; shift++) {
        int doctor_id = schedule[day][shift];
        const char *doctor_name =
            getDoctorNameById(doctor_id, data, number_doctor);

        printf("\u2502 %-11s \u2502 [ID:%02d] %-38s \u2502\n",
               shift_name[shift], doctor_id, doctor_name);
    }

    printf("\u2514\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500"
           "\u2500\u2500\u2500\u2534\u2500\u2500\u2500\u2500\u2500\u2500\u2500"
           "\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500"
           "\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500"
           "\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500"
           "\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2518"
           "\n\n");
}

/*
 * Menampilkan jadwal mingguan untuk minggu tertentu
 * Input: week (0-4), schedule matrix, data dokter, jumlah dokter
 */
void displayWeeklySchedule(int week, int schedule[30][3], Doctor **data,
                           int number_doctor) {
    char title[100];
    sprintf(title, "JADWAL JAGA MINGGU KE-%d", week + 1);

    printScheduleHeader(title);

    int start_day = week * 7;
    int end_day = (week + 1) * 7;
    if (end_day > 30)
        end_day = 30;

    // Header tabel
    printf("\u250c\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500"
           "\u252c\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500"
           "\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u252c\u2500"
           "\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500"
           "\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u252c\u2500\u2500\u2500"
           "\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500"
           "\u2500\u2500\u2500\u2500\u2500\u2510\n");
    printf("\u2502   HARI   \u2502       PAGI        \u2502      SIANG        "
           "\u2502      MALAM        \u2502\n");
    printf("\u251c\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500"
           "\u253c\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500"
           "\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u253c\u2500"
           "\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500"
           "\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u253c\u2500\u2500\u2500"
           "\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500"
           "\u2500\u2500\u2500\u2500\u2500\u2524\n");

    for (int day = start_day; day < end_day; day++) {
        printf("\u2502 %-8s \u2502", days_name[day % 7]);

        for (int shift = 0; shift < 3; shift++) {
            int doctor_id = schedule[day][shift];
            const char *doctor_name =
                getDoctorNameById(doctor_id, data, number_doctor);

            // Potong nama jika terlalu panjang
            char short_name[16];
            if (strlen(doctor_name) > 15) {
                strncpy(short_name, doctor_name, 12);
                short_name[12] = '.';
                short_name[13] = '.';
                short_name[14] = '.';
                short_name[15] = '\0';
            } else {
                strcpy(short_name, doctor_name);
            }

            printf(" [%02d] %-11s \u2502", doctor_id, short_name);
        }
        printf("\n");
    }

    printf("\u2514\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500"
           "\u2534\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500"
           "\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2534\u2500"
           "\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500"
           "\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2534\u2500\u2500\u2500"
           "\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500"
           "\u2500\u2500\u2500\u2500\u2500\u2518\n\n");
}

/*
 * Menampilkan jadwal bulanan (keseluruhan 30 hari)
 * Input: schedule matrix, data dokter, jumlah dokter
 */
void displayMonthlySchedule(int schedule[30][3], Doctor **data,
                            int number_doctor) {
    printScheduleHeader("JADWAL JAGA BULANAN (30 HARI)");

    printf("\u250c\u2500\u2500\u2500\u2500\u2500\u2500\u252c\u2500\u2500\u2500"
           "\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u252c\u2500\u2500\u2500"
           "\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500"
           "\u2500\u2500\u2500\u2500\u2500\u252c\u2500\u2500\u2500\u2500\u2500"
           "\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500"
           "\u2500\u2500\u2500\u252c\u2500\u2500\u2500\u2500\u2500\u2500\u2500"
           "\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500"
           "\u2500\u2510\n");
    printf("\u2502 HARI \u2502   TGL    \u2502       PAGI        \u2502      "
           "SIANG        \u2502      MALAM        \u2502\n");
    printf("\u251c\u2500\u2500\u2500\u2500\u2500\u2500\u253c\u2500\u2500\u2500"
           "\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u253c\u2500\u2500\u2500"
           "\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500"
           "\u2500\u2500\u2500\u2500\u2500\u253c\u2500\u2500\u2500\u2500\u2500"
           "\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500"
           "\u2500\u2500\u2500\u253c\u2500\u2500\u2500\u2500\u2500\u2500\u2500"
           "\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500"
           "\u2500\u2524\n");

    for (int day = 0; day < 30; day++) {
        printf("\u2502 %-4s \u2502   %02d     \u2502", days_name[day % 7],
               day + 1);

        for (int shift = 0; shift < 3; shift++) {
            int doctor_id = schedule[day][shift];
            const char *doctor_name =
                getDoctorNameById(doctor_id, data, number_doctor);

            // Potong nama jika terlalu panjang
            char short_name[16];
            if (strlen(doctor_name) > 15) {
                strncpy(short_name, doctor_name, 12);
                short_name[12] = '.';
                short_name[13] = '.';
                short_name[14] = '.';
                short_name[15] = '\0';
            } else {
                strcpy(short_name, doctor_name);
            }

            printf(" [%02d] %-11s \u2502", doctor_id, short_name);
        }
        printf("\n");

        // Tambahkan separator setiap minggu
        if ((day + 1) % 7 == 0 && day < 29) {
            printf(
                "\u251c\u2500\u2500\u2500\u2500\u2500\u2500\u253c\u2500\u2500"
                "\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u253c\u2500"
                "\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500"
                "\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u253c\u2500"
                "\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500"
                "\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u253c\u2500"
                "\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500"
                "\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2524\n");
        }
    }

    printf("\u2514\u2500\u2500\u2500\u2500\u2500\u2500\u2534\u2500\u2500\u2500"
           "\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2534\u2500\u2500\u2500"
           "\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500"
           "\u2500\u2500\u2500\u2500\u2500\u2534\u2500\u2500\u2500\u2500\u2500"
           "\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500"
           "\u2500\u2500\u2500\u2534\u2500\u2500\u2500\u2500\u2500\u2500\u2500"
           "\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500"
           "\u2500\u2518\n\n");
}

/*
 * Menampilkan statistik shift per dokter
 * Input: schedule matrix, data dokter, jumlah dokter
 */
void displayShiftStatistics(int schedule[30][3], Doctor **data,
                            int number_doctor) {
    printScheduleHeader("STATISTIK SHIFT PER DOKTER");

    // Hitung total shift per dokter
    int total_shifts[number_doctor +
                     1]; // +1 untuk mengakomodasi ID mulai dari 1
    int shifts_per_week[number_doctor + 1][5]; // 5 minggu

    // Inisialisasi array
    for (int i = 0; i <= number_doctor; i++) {
        total_shifts[i] = 0;
        for (int week = 0; week < 5; week++) {
            shifts_per_week[i][week] = 0;
        }
    }

    // Hitung shift
    for (int day = 0; day < 30; day++) {
        int week = day / 7;
        for (int shift = 0; shift < 3; shift++) {
            int doctor_id = schedule[day][shift];
            if (doctor_id > 0 && doctor_id <= number_doctor) {
                total_shifts[doctor_id]++;
                shifts_per_week[doctor_id][week]++;
            }
        }
    }

    printf("\u250c\u2500\u2500\u2500\u2500\u2500\u252c\u2500\u2500\u2500\u2500"
           "\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500"
           "\u2500\u2500\u2500\u2500\u2500\u2500\u252c\u2500\u2500\u2500\u2500"
           "\u2500\u2500\u2500\u252c\u2500\u2500\u2500\u2500\u2500\u2500\u2500"
           "\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500"
           "\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500"
           "\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2510\n");
    printf("\u2502 ID  \u2502        NAMA         \u2502 TOTAL \u2502        "
           "SHIFT PER MINGGU              \u2502\n");
    printf("\u2502     \u2502                     \u2502 SHIFT \u2502   1    2 "
           "   3    4    5    MAX/WEEK  \u2502\n");
    printf("\u251c\u2500\u2500\u2500\u2500\u2500\u253c\u2500\u2500\u2500\u2500"
           "\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500"
           "\u2500\u2500\u2500\u2500\u2500\u2500\u253c\u2500\u2500\u2500\u2500"
           "\u2500\u2500\u2500\u253c\u2500\u2500\u2500\u2500\u2500\u2500\u2500"
           "\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500"
           "\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500"
           "\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2524\n");

    for (int i = 1; i <= number_doctor; i++) {
        if (data[i - 1] != NULL) {
            printf("\u2502 %2d  \u2502 %-19s \u2502  %2d   \u2502", i,
                   data[i - 1]->name, total_shifts[i]);

            // Tampilkan shift per minggu
            for (int week = 0; week < 5; week++) {
                printf("  %2d ", shifts_per_week[i][week]);
            }
            printf("    %2d      \u2502\n", data[i - 1]->max_shift_per_week);
        }
    }

    printf("\u2514\u2500\u2500\u2500\u2500\u2500\u2534\u2500\u2500\u2500\u2500"
           "\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500"
           "\u2500\u2500\u2500\u2500\u2500\u2500\u2534\u2500\u2500\u2500\u2500"
           "\u2500\u2500\u2500\u2534\u2500\u2500\u2500\u2500\u2500\u2500\u2500"
           "\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500"
           "\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500"
           "\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2518\n\n");
}

/*
 * Menampilkan informasi pelanggaran preferensi dokter
 * Input: conflict linked list
 */
void displayPreferenceViolations(conflict *head) {
    printScheduleHeader("PELANGGARAN PREFERENSI DOKTER");

    if (head == NULL) {
        printf(
            "\U0001f389 SELAMAT! Tidak ada pelanggaran preferensi dokter.\n");
        printf(
            "   Semua dokter dijadwalkan sesuai dengan preferensi mereka.\n\n");
        return;
    }

    printf(
        "\u26a0\ufe0f  Ditemukan pelanggaran preferensi dokter berikut:\n\n");

    printf(
        "\u250c\u2500\u2500\u2500\u2500\u2500\u252c\u2500\u2500\u2500\u2500"
        "\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500"
        "\u2500\u2500\u2500\u2500\u2500\u2500\u252c\u2500\u2500\u2500\u2500"
        "\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u252c"
        "\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500"
        "\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2510\n");
    printf("\u2502 ID  \u2502        NAMA         \u2502     HARI     \u2502   "
           "    SHIFT         \u2502\n");
    printf(
        "\u251c\u2500\u2500\u2500\u2500\u2500\u253c\u2500\u2500\u2500\u2500"
        "\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500"
        "\u2500\u2500\u2500\u2500\u2500\u2500\u253c\u2500\u2500\u2500\u2500"
        "\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u253c"
        "\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500"
        "\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2524\n");

    conflict *temp = head;
    int violation_count = 0;

    while (temp != NULL) {
        int day_index = (temp->hari - 1) % 7;
        printf("\u2502 %2d  \u2502 %-19s \u2502 Hari ke-%-4d \u2502 %-19s "
               "\u2502\n",
               temp->id, temp->name, temp->hari, shift_name[temp->shift - 1]);
        printf("\u2502     \u2502                     \u2502 (%s)       \u2502 "
               "                    \u2502\n",
               days_name[day_index]);

        violation_count++;
        temp = temp->next;

        if (temp != NULL) {
            printf(
                "\u251c\u2500\u2500\u2500\u2500\u2500\u253c\u2500\u2500\u2500"
                "\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500"
                "\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u253c\u2500"
                "\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500"
                "\u2500\u2500\u2500\u253c\u2500\u2500\u2500\u2500\u2500\u2500"
                "\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500"
                "\u2500\u2500\u2500\u2500\u2500\u2524\n");
        }
    }

    printf(
        "\u2514\u2500\u2500\u2500\u2500\u2500\u2534\u2500\u2500\u2500\u2500"
        "\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500"
        "\u2500\u2500\u2500\u2500\u2500\u2500\u2534\u2500\u2500\u2500\u2500"
        "\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2534"
        "\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500"
        "\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2500\u2518\n");
    printf("\n\U0001f4ca Total Pelanggaran: %d shift\n", violation_count);
    printf("\U0001f4a1 Saran: Pertimbangkan untuk menambah dokter atau "
           "menyesuaikan preferensi.\n\n");
}

/*
 * Menu utama untuk tampilan jadwal
 */
void scheduleViewMenu(int schedule[30][3], Doctor **data, int number_doctor,
                      conflict *conflict_head) {
    int choice, day, week;

    while (1) {
        printf("\n");
        printScheduleHeader("MENU TAMPILAN JADWAL");
        printf("1. Tampilkan Jadwal Harian\n");
        printf("2. Tampilkan Jadwal Mingguan\n");
        printf("3. Tampilkan Jadwal Bulanan\n");
        printf("4. Statistik Shift per Dokter\n");
        printf("5. Pelanggaran Preferensi\n");
        printf("6. Kembali ke Menu Utama\n");
        printf("Pilihan Anda: ");

        if (scanf("%d", &choice) != 1) {
            printf("\u274c Input tidak valid! Masukkan angka.\n");
            while (getchar() != '\n')
                ; // Bersihkan buffer
            continue;
        }

        switch (choice) {
        case 1:
            printf("Masukkan hari ke- (1-30): ");
            if (scanf("%d", &day) == 1 && day >= 1 && day <= 30) {
                displayDailySchedule(day - 1, schedule, data, number_doctor);
            } else {
                printf("\u274c Hari tidak valid! Masukkan angka 1-30.\n");
            }
            break;

        case 2:
            printf("Masukkan minggu ke- (1-5): ");
            if (scanf("%d", &week) == 1 && week >= 1 && week <= 5) {
                displayWeeklySchedule(week - 1, schedule, data, number_doctor);
            } else {
                printf("\u274c Minggu tidak valid! Masukkan angka 1-5.\n");
            }
            break;

        case 3:
            displayMonthlySchedule(schedule, data, number_doctor);
            break;

        case 4:
            displayShiftStatistics(schedule, data, number_doctor);
            break;

        case 5:
            displayPreferenceViolations(conflict_head);
            break;

        case 6:
            printf("Kembali ke menu utama...\n");
            return;

        default:
            printf("\u274c Pilihan tidak valid! Pilih angka 1-6.\n");
        }

        printf("\nTekan Enter untuk melanjutkan...");
        while (getchar() != '\n')
            ;      // Bersihkan buffer
        getchar(); // Tunggu Enter
    }
}
