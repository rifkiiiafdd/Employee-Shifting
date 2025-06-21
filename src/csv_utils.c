/*
 * File: csv_utils.c
 * Deskripsi: Implementasi fungsi-fungsi untuk membaca dan menulis data
 * ke/dari file CSV (Comma-Separated Values)
 */

#include "csv_utils.h"
#include "doctor_list.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ----- FUNGSI UNTUK FILE DOKTER -----

// Menyimpan data dokter yang ada di memori (linked list) ke file CSV
bool save_doctors_to_csv(const char *filepath) {
    FILE *file = fopen(filepath, "w");
    if (file == NULL) {
        perror("Gagal membuka file untuk penulisan data dokter");
        return false;
    }

    // Tulis baris header terlebih dahulu
    fprintf(file, "ID,Name,MaxShifts,Week1,Week2,Week3,Week4,Week5");
    // Tambahkan header untuk setiap kemungkinan preferensi
    for (int d = 0; d < NUM_DAYS_PER_WEEK; d++) {
        for (int s = 0; s < NUM_SHIFTS_PER_DAY; s++) {
            fprintf(file, ",Pref_D%dS%d", d, s);
        }
    }
    fprintf(file, "\n");

    // Tulis data untuk setiap dokter di linked list
    Doctor *current = head;
    while (current != NULL) {
        fprintf(file, "%d,%s,%d", current->id, current->name,
                current->max_shifts_per_week);
        // Tulis data shift per minggu
        for (int i = 0; i < NUM_WEEKS; i++) {
            fprintf(file, ",%d", current->shifts_scheduled_per_week[i]);
        }
        // Tulis data preferensi
        for (int d = 0; d < NUM_DAYS_PER_WEEK; d++) {
            for (int s = 0; s < NUM_SHIFTS_PER_DAY; s++) {
                fprintf(file, ",%d", current->preference[d][s]);
            }
        }
        fprintf(file, "\n");
        current = current->next;
    }

    fclose(file);
    return true;
}

// Memuat data dokter dari file CSV ke memori
// Data dokter yang ada di memori saat ini akan dihapus total
bool load_doctors_from_csv(const char *filepath) {
    FILE *file = fopen(filepath, "r");
    if (file == NULL) {
        // Ini bukan error, mungkin file memang belum ada. Jadi tidak perlu
        // output error
        return false;
    }

    freeDoctorList(); // Kosongkan list dokter yang ada di memori

    char line[1024]; // Buffer untuk membaca satu baris dari file
    // Baca dan buang baris header
    if (fgets(line, sizeof(line), file) == NULL) {
        fclose(file);
        return true; // File kosong (hanya header), bukan error
    }

    // Baca baris per baris data dokter
    while (fgets(line, sizeof(line), file)) {
        if (strlen(line) < 2)
            continue; // Lewati baris kosong

        Doctor *new_doctor = (Doctor *)malloc(sizeof(Doctor));
        if (new_doctor == NULL) {
            perror("Alokasi memori gagal saat memuat dokter dari CSV");
            fclose(file);
            return false;
        }
        new_doctor->next = NULL;

        // Gunakan strtok untuk memecah baris berdasarkan koma
        char *token = strtok(line, ",\n");
        int field_count = 0;
        int total_prefs = NUM_DAYS_PER_WEEK * NUM_SHIFTS_PER_DAY;

        while (token != NULL) {
            // Isi struct Doctor berdasarkan urutan kolom
            switch (field_count) {
            case 0: // Kolom ID
                new_doctor->id = atoi(token);
                break;
            case 1: // Kolom Nama
                strncpy(new_doctor->name, token, MAX_NAME_LENGTH - 1);
                new_doctor->name[MAX_NAME_LENGTH - 1] = '\0';
                break;
            case 2: // Kolom MaxShifts
                new_doctor->max_shifts_per_week = atoi(token);
                break;
            default: // Kolom-kolom sisanya (shift per minggu dan preferensi)
                if (field_count >= 3 && field_count < 3 + NUM_WEEKS) {
                    // Ini kolom shift per minggu
                    new_doctor->shifts_scheduled_per_week[field_count - 3] =
                        atoi(token);
                } else if (field_count >= 3 + NUM_WEEKS &&
                           field_count < 3 + NUM_WEEKS + total_prefs) {
                    // Ini kolom preferensi
                    int pref_index = field_count - (3 + NUM_WEEKS);
                    int day = pref_index / NUM_SHIFTS_PER_DAY;
                    int shift = pref_index % NUM_SHIFTS_PER_DAY;
                    if (day < NUM_DAYS_PER_WEEK && shift < NUM_SHIFTS_PER_DAY)
                        new_doctor->preference[day][shift] = atoi(token);
                }
                break;
            }
            token = strtok(NULL, ",\n"); // Ambil token berikutnya
            field_count++;
        }

        // Masukkan dokter yang baru dibuat ke linked list
        if (head == NULL) {
            head = new_doctor;
        } else {
            Doctor *current = head;
            while (current->next != NULL)
                current = current->next;
            current->next = new_doctor;
        }
    }

    fclose(file);
    refreshDoctorIDandSchedule();
    return true;
}

// ----- FUNGSI UNTUK FILE JADWAL -----

// Menyimpan data jadwal dari array global 'schedule' ke file CSV.
bool save_schedule_to_csv(const char *filepath) {
    FILE *file = fopen(filepath, "w");
    if (file == NULL) {
        perror("Gagal membuka file untuk penulisan jadwal");
        return false;
    }

    // Tulis header
    fprintf(file, "DayIndex,Morning,Afternoon,Night\n");

    // Tulis data jadwal untuk 30 hari
    for (int d = 0; d < NUM_DAYS; d++) {
        fprintf(file, "%d,%d,%d,%d\n", d, schedule[d][0], schedule[d][1],
                schedule[d][2]);
    }

    fclose(file);
    return true;
}

// Memuat data jadwal dari file CSV ke array global 'schedule'.
bool load_schedule_from_csv(const char *filepath) {
    FILE *file = fopen(filepath, "r");
    if (file == NULL) {
        return false; // File mungkin belum ada, tidak apa-apa.
    }

    initialize_schedule(); // Kosongkan jadwal di memori terlebih dahulu

    char line[256];
    // Baca dan buang baris header
    if (fgets(line, sizeof(line), file) == NULL) {
        fclose(file);
        return true; // File kosong, tidak masalah.
    }

    int d, m_id, a_id, n_id;
    // Baca file per baris, format: int,int,int,int
    while (fscanf(file, "%d,%d,%d,%d", &d, &m_id, &a_id, &n_id) == 4) {
        // Pastikan indeks hari valid sebelum mengisi array
        if (d >= 0 && d < NUM_DAYS) {
            schedule[d][0] = m_id; // Shift Pagi
            schedule[d][1] = a_id; // Shift Siang (Afternoon)
            schedule[d][2] = n_id; // Shift Malam (Night)
        }
    }

    fclose(file);
    return true;
}
