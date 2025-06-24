// TEMPORARY MAIN UNTUK CLI
#include "include/csv_utils.h"
#include "include/doctor_list.h"
#include "include/scheduler.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>

#ifdef _WIN32
#define CLEAR_SCREEN "cls"
#define PATH_SEPARATOR '\\'
#else
#define CLEAR_SCREEN "clear"
#define PATH_SEPARATOR '/'
#endif

// definisi lebar kolom untuk tampil jadwal
#define DAY_LABEL_WIDTH 20
#define SHIFT_COL_WIDTH 18

// prototype fungsi core
void run_cli_app();
void handle_schedule_display();
void handle_doctor_management();
void handle_data_operations();

// fungsi penunjang
int get_int(const char *prompt);
void get_str(const char *prompt, char *buf, int size);
bool confirm(const char *prompt);
Doctor *select_doctor();

int main() {
    srand(time(NULL));
    initialize_schedule();
    run_cli_app();

    freeDoctorList();
    free_conflict_list();
    printf("Memori telah dibebaskan. Sampai jumpa!\n");
    return 0;
}

void run_cli_app() {
    int choice;
    while (1) {
        system(CLEAR_SCREEN);
        printf(
            "   _____      _              _       _             __   ___  \n");
        printf(
            "  / ____|    | |            | |     | |           /_ | / _ \\ \n");
        printf(
            " | (___   ___| |__   ___  __| |_   _| | ___ _ __   | || | | |\n");
        printf("  \\___ \\ / __| '_ \\ / _ \\/ _` | | | | |/ _ \\ '__|  | || | "
               "| |\n");
        printf(
            "  ____) | (__| | | |  __/ (_| | |_| | |  __/ |     | || |_| |\n");
        printf(" |_____/ \\___|_| |_|\\___|\\__,_|\\__,_|_|\\___|_|     "
               "|_(_)___/ \n");

        printf("\n--- Menu Utama Penjadwal Dokter ---\n"
               "1. Tampilkan Semua Dokter\n"
               "2. Kelola Dokter\n"
               "3. Buat Jadwal Otomatis\n"
               "4. Tampilkan Jadwal\n"
               "5. Tampilkan Konflik Jadwal\n"
               "6. Kelola Data\n"
               "7. Keluar\n"
               "----------------------------------\n");

        choice = get_int("Masukkan pilihan Anda: ");

        switch (choice) {
        case 1:
            displayDoctors();
            break;
        case 2:
            handle_doctor_management();
            break;
        case 3:
            printf("Membuat Jadwal...\n");
            generate_schedule();
            printf("Pembuatan jadwal selesai.\n");
            break;
        case 4:
            handle_schedule_display();
            break;
        case 5:
            view_conflict();
            break;
        case 6:
            handle_data_operations();
            break;
        case 7:
            return;
        default:
            printf("\n*** Pilihan tidak valid ***\n");
        }

        if (choice != 7) {
            printf("\nTekan Enter untuk melanjutkan...");
            getchar();
        }
    }
}

void handle_schedule_display() {
    system(CLEAR_SCREEN);
    printf("\n--- Menu Tampilan Jadwal ---\n"
           "1. Tampilkan Jadwal Keseluruhan (30 Hari)\n"
           "2. Tampilkan Jadwal Per Minggu\n"
           "3. Tampilkan Jadwal Per Hari\n"
           "4. Tampilkan Jadwal Dokter (30 Hari)\n"
           "5. Tampilkan Jadwal Dokter Per Minggu\n"
           "6. Tampilkan Jadwal Dokter Per Hari\n"
           "----------------------------\n");

    int choice = get_int("Masukkan pilihan: ");
    if (choice < 1 || choice > 6) {
        printf("\n*** Pilihan tidak valid ***\n");
        return;
    }

    int week, day;
    Doctor *doc = NULL;
    const char *days[] = {"Senin", "Selasa", "Rabu",  "Kamis",
                          "Jumat", "Sabtu",  "Minggu"};
    const char *shifts[] = {"Pagi", "Siang", "Malam"};

    switch (choice) {
    case 1: { // Penampil jadwal bulan
        int sched[NUM_DAYS][NUM_SHIFTS_PER_DAY];
        get_all_doctors_schedule_month(sched);

        printf("\n--- JADWAL KESELURUHAN (30 HARI) ---\n");
        for (int w = 0; w < NUM_WEEKS; w++) {
            printf("\n--- MINGGU %d ---\n", w + 1);
            printf("%-*s", DAY_LABEL_WIDTH, "Hari");
            for (int s = 0; s < NUM_SHIFTS_PER_DAY; s++) {
                printf("%-*s", SHIFT_COL_WIDTH, shifts[s]);
            }
            printf("\n");

            for (int d = 0; d < NUM_DAYS_PER_WEEK; d++) {
                int current_day = w * NUM_DAYS_PER_WEEK + d;
                if (current_day >= NUM_DAYS)
                    break;

                char day_label[DAY_LABEL_WIDTH];
                strcpy(day_label, "Hari ");
                char day_num[4];
                sprintf(day_num, "%d", current_day + 1);
                strcat(day_label, day_num);
                strcat(day_label, " (");
                strcat(day_label, days[d]);
                strcat(day_label, ")");
                printf("%-*s", DAY_LABEL_WIDTH, day_label);

                for (int s = 0; s < NUM_SHIFTS_PER_DAY; s++) {
                    Doctor *assigned_doc =
                        findDoctorById(sched[current_day][s]);
                    char display_buffer[MAX_NAME_LENGTH + 4]; // "dr. " + name

                    if (assigned_doc != NULL) {
                        strcpy(display_buffer, "dr. ");
                        strcat(display_buffer, assigned_doc->name);
                        printf("%-*s", SHIFT_COL_WIDTH, display_buffer);
                    } else {
                        printf("%-*s", SHIFT_COL_WIDTH, "Kosong");
                    }
                }
                printf("\n");
            }
        }
        break;

    case 2: { // Jadwal Minggu
        week = get_int("Masukkan minggu (1-5): ");
        if (week < 1 || week > NUM_WEEKS) {
            printf("Minggu tidak valid\n");
            return;
        }

        int weekly_sched[NUM_DAYS_PER_WEEK][NUM_SHIFTS_PER_DAY];
        get_all_doctors_schedule_week(week, weekly_sched);

        printf("\n--- JADWAL MINGGU %d ---\n", week);
        printf("%-*s", DAY_LABEL_WIDTH, "Hari");
        for (int s = 0; s < NUM_SHIFTS_PER_DAY; s++) {
            printf("%-*s", SHIFT_COL_WIDTH, shifts[s]);
        }
        printf("\n");

        for (int d = 0; d < NUM_DAYS_PER_WEEK; d++) {
            printf("%-*s", DAY_LABEL_WIDTH, days[d]);
            for (int s = 0; s < NUM_SHIFTS_PER_DAY; s++) {
                Doctor *assigned_doc = findDoctorById(weekly_sched[d][s]);
                char display_buffer[MAX_NAME_LENGTH + 4]; // "dr. " + name

                if (assigned_doc != NULL) {
                    strcpy(display_buffer, "dr. ");
                    strcat(display_buffer, assigned_doc->name);
                    printf("%-*s", SHIFT_COL_WIDTH, display_buffer);
                } else {
                    printf("%-*s", SHIFT_COL_WIDTH, "Kosong");
                }
            }
            printf("\n");
        }
        break;
    }

    case 3: { // Jadwal Harian
        week = get_int("Masukkan minggu (1-5): ");
        day = get_int("Masukkan hari (1-7): ") - 1;
        if (week < 1 || week > NUM_WEEKS || day < 0 ||
            day >= NUM_DAYS_PER_WEEK) {
            printf("Input tidak valid\n");
            return;
        }

        int daily_sched[NUM_SHIFTS_PER_DAY];
        get_all_doctors_schedule_day(week, day, daily_sched);

        printf("\n--- Jadwal %s, Minggu %d ---\n", days[day], week);
        for (int s = 0; s < NUM_SHIFTS_PER_DAY; s++) {
            Doctor *doc = findDoctorById(daily_sched[s]);
            if (doc != NULL) {
                char display_buffer[MAX_NAME_LENGTH + 4];
                strcpy(display_buffer, "dr. ");
                strcat(display_buffer, doc->name);
                printf("  %-7s: %s\n", shifts[s], display_buffer);
            } else {
                printf("  %-7s: %s\n", shifts[s], "Kosong");
            }
        }
        break;
    }
    case 4: { // Jadwal dokter spesifik bulan
        doc = select_doctor();
        if (!doc)
            return;

        int sched[NUM_WEEKS][NUM_DAYS_PER_WEEK][NUM_SHIFTS_PER_DAY];
        int counts[NUM_WEEKS][NUM_DAYS_PER_WEEK];
        get_doctor_schedule_month(doc->id, sched, counts);

        printf("\n--- Jadwal Bulanan dr. %s ---\n", doc->name);
        for (int w = 0; w < NUM_WEEKS; w++) {
            if (totalShiftWeek(w + 1, doc->id) > 0) {
                printf("\n--- Minggu %d ---\n", w + 1);
                for (int d = 0; d < NUM_DAYS_PER_WEEK; d++) {
                    if (counts[w][d] > 0) {
                        printf("%-7s: ", days[d]);
                        for (int s = 0; s < counts[w][d]; s++) {
                            printf("%.*s ", SHIFT_COL_WIDTH,
                                   shifts[sched[w][d][s]]);
                        }
                        printf("\n");
                    }
                }
            }
        }
        printf("\nTotal Shift Sebulan: %d\n", totalShift(doc->id));
        break;
    }
    case 5: { // Jadwal dokter spesifik minggu
        doc = select_doctor();
        if (!doc)
            return;

        week = get_int("Masukkan minggu (1-5): ");
        if (week < 1 || week > NUM_WEEKS)
            return;

        int sched[NUM_DAYS_PER_WEEK][NUM_SHIFTS_PER_DAY];
        int counts[NUM_DAYS_PER_WEEK];
        getDoctorScheduleWeek(week, doc->id, sched, counts);

        printf("\n--- Jadwal dr. %s - Minggu %d ---\n", doc->name, week);
        for (int d = 0; d < NUM_DAYS_PER_WEEK; d++) {
            if (counts[d] > 0) {
                printf("%-7s: ", days[d]);
                for (int s = 0; s < counts[d]; s++) {
                    printf("%.*s ", SHIFT_COL_WIDTH, shifts[sched[d][s]]);
                }
                printf("\n");
            }
        }
        printf("\nTotal Shift Minggu %d: %d\n", week,
               totalShiftWeek(week, doc->id));
        break;
    }
    case 6: { // Jadwal dokter spesifik harian
        doc = select_doctor();
        if (!doc)
            return;

        week = get_int("Masukkan minggu (1-5): ");
        day = get_int("Masukkan hari (1-7): ") - 1;
        if (week < 1 || week > NUM_WEEKS || day < 0 || day >= NUM_DAYS_PER_WEEK)
            return;

        int sched[NUM_SHIFTS_PER_DAY];
        int count;
        getDoctorScheduleDay(week, day, doc->id, sched, &count);

        printf("\n--- Jadwal dr. %s - %s, Minggu %d ---\n", doc->name,
               days[day], week);
        if (count == 0) {
            printf("Tidak ada jadwal shift pada hari ini.\n");
        } else {
            printf("Bertugas pada shift:\n");
            for (int i = 0; i < count; i++) {
                printf("%.*s ", SHIFT_COL_WIDTH, shifts[sched[i]]);
            }
        }
        printf("\nTotal Shift %s, Minggu %d: %d\n", days[day], week,
               totalShiftDay(week, day, doc->id));
        break;
    }
    }
    }
}

void handle_doctor_management() {
    system(CLEAR_SCREEN);
    printf("\n--- Kelola Dokter ---\n"
           "1. Tambah Dokter\n"
           "2. Edit Dokter\n"
           "3. Hapus Dokter\n"
           "4. Kembali\n"
           "-------------------\n");

    int choice = get_int("Pilihan: ");
    if (choice < 1 || choice > 3)
        return;

    Doctor *doc;
    char name[MAX_NAME_LENGTH];
    int max_shifts;

    switch (choice) {
    case 1: // Add
        get_str("Nama dokter: ", name, MAX_NAME_LENGTH);
        max_shifts = get_int("Maks shift per minggu: ");
        if (strlen(name) && max_shifts > 0) {
            addDoctor(name, max_shifts);
            printf("Dokter ditambahkan\n");
        }
        doc = head;
        while (doc->next != NULL) {
            doc = doc->next;
        }
        printf("\n--- Preferensi dr. %s ---\n", doc->name);
        while (1) {
            int day = get_int("Hari (1-7, 0 untuk selesai): ");
            if (day == 0)
                break;
            if (day < 1 || day > 7)
                continue;

            day--;

            int shift = get_int("Shift (1-3): ") - 1;
            if (shift < 0 || shift >= NUM_SHIFTS_PER_DAY)
                continue;

            int pref = get_int("Tersedia? (1=Ya, 0=Tidak): ");
            if (pref == 0 || pref == 1) {
                setDoctorPreference(doc->id, day, shift, pref);
            }
        }
        break;

    case 2: // Edit
        doc = select_doctor();
        if (!doc)
            return;

        get_str("Nama baru (Enter untuk skip): ", name, MAX_NAME_LENGTH);
        max_shifts = get_int("Maks shift baru (0 untuk skip): ");
        updateDoctor(doc->id, strlen(name) ? name : NULL,
                     max_shifts > 0 ? max_shifts : -1);
        printf("Dokter diperbarui\n");
        printf("\n--- Preferensi dr. %s ---\n", doc->name);
        while (1) {
            int day = get_int("Hari (1-7, 0 untuk selesai): ");
            if (day == 0)
                break;
            if (day < 1 || day > 7)
                continue;

            day--;

            int shift = get_int("Shift (1-3): ") - 1;
            if (shift < 0 || shift >= NUM_SHIFTS_PER_DAY)
                continue;

            int pref = get_int("Tersedia? (1=Ya, 0=Tidak): ");
            if (pref == 0 || pref == 1) {
                setDoctorPreference(doc->id, day, shift, pref);
            }
        }

        break;

    case 3: // Remove
        doc = select_doctor();
        if (doc && confirm("Yakin hapus dokter? (y/n): ")) {
            removeDoctor(doc->id);
            printf("Dokter dihapus\n");
        }
        break;
    }
}

void handle_data_operations() {
    system(CLEAR_SCREEN);
    printf("\n--- Kelola Data ---\n"
           "1. Load Data\n"
           "2. Save Data\n"
           "3. Kembali\n"
           "-----------------\n");

    int choice = get_int("Pilihan: ");
    if (choice < 1 || choice > 2)
        return;

    char path[256];
    get_str("Path direktori (Enter untuk 'data'): ", path, 256);
    if (strlen(path) == 0) {
        strcpy(path, "data");
    }

    // Buat direktori kalau gak ada
    struct stat st = {0};
    if (stat(path, &st) == -1) {
#ifdef _WIN32
        _mkdir(path);
#else
        mkdir(path, 0777);
#endif
    }

    // file path
    char doc_path[512];
    char sched_path[512];
    snprintf(doc_path, sizeof(doc_path), "%s%cdoctors.csv", path,
             PATH_SEPARATOR);
    snprintf(sched_path, sizeof(sched_path), "%s%cschedule.csv", path,
             PATH_SEPARATOR);

    if (choice == 1) { // Load
        printf("Memuat data dari:\n- %s\n- %s\n", doc_path, sched_path);

        if (load_doctors_from_csv(doc_path)) {
            printf("Data dokter berhasil dimuat.\n");
        } else {
            printf("Gagal memuat data dokter.\n");
        }
        if (load_schedule_from_csv(sched_path)) {
            printf("Jadwal berhasil dimuat.\n");
        } else {
            printf("Gagal memuat jadwal.\n");
        }
    } else { // Save
        printf("Menyimpan data ke:\n- %s\n- %s\n", doc_path, sched_path);

        if (save_doctors_to_csv(doc_path)) {
            printf("Data dokter berhasil disimpan.\n");
        } else {
            printf("Gagal menyimpan data dokter.\n");
        }
        if (save_schedule_to_csv(sched_path)) {
            printf("Jadwal berhasil disimpan.\n");
        } else {
            printf("Gagal menyimpan jadwal.\n");
        }
    }
}

int get_int(const char *prompt) {
    char buf[100];
    int c;
    printf("%s", prompt);
    fgets(buf, sizeof(buf), stdin);
    char *newline = strchr(buf, '\n');
    if (newline) {
        *newline = '\0';
    } else {
        while ((c = getchar()) != '\n' && c != EOF)
            ;
    }
    return atoi(buf);
}

void get_str(const char *prompt, char *buf, int size) {
    printf("%s", prompt);
    fgets(buf, size, stdin);
    buf[strcspn(buf, "\n")] = 0;
}

bool confirm(const char *prompt) {
    char buf[10];
    printf("%s ", prompt);
    fgets(buf, sizeof(buf), stdin);
    return tolower(buf[0]) == 'y';
}

Doctor *select_doctor() {
    displayDoctors();
    int id = get_int("\nPilih ID dokter: ");
    Doctor *doc = findDoctorById(id);
    if (!doc)
        printf("Dokter tidak ditemukan\n");
    return doc;
}
