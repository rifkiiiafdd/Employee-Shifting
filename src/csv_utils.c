#include "csv_utils.h"
#include "doctor_list.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Fungsi CSV List Dokter

// Menyimpan data dokter ke CSV
bool save_doctors_to_csv(const char *filepath) {
    FILE *file = fopen(filepath, "w");
    if (file == NULL) {
        perror("Error opening file for writing");
        return false;
    }

    // Buat line header
    fprintf(file, "ID,Name,MaxShifts,Week1,Week2,Week3,Week4,Week5");
    for (int d = 0; d < NUM_DAYS_PER_WEEK; d++) {
        for (int s = 0; s < NUM_SHIFTS_PER_DAY; s++) {
            fprintf(file, ",Pref_D%dS%d", d, s);
        }
    }
    fprintf(file, "\n");

    // Buat line untuk setiap dokter
    Doctor *current = head;
    while (current != NULL) {
        fprintf(file, "%d,%s,%d", current->id, current->name,
                current->max_shifts_per_week);
        for (int i = 0; i < NUM_WEEKS; i++) {
            fprintf(file, ",%d", current->shifts_scheduled_per_week[i]);
        }
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

// Load data dokter dari CSV
bool load_doctors_from_csv(const char *filepath) {
    FILE *file = fopen(filepath, "r");
    if (file == NULL) {
        return false; // File tidak ada, bukan error
    }

    freeDoctorList(); // Kosongkan data dokter sebelum diload selanjutnya

    char line[1024];
    // Skip line header
    if (fgets(line, sizeof(line), file) == NULL) {
        fclose(file);
        return true; // File kosong, bukan error
    }

    while (fgets(line, sizeof(line), file)) {
        if (strlen(line) < 2)
            continue; // Error handling untuk line dibawah threshold jumlah
                      // karakter

        Doctor *new_doctor = (Doctor *)malloc(sizeof(Doctor));
        if (new_doctor == NULL) {
            perror("Memory allocation failed during CSV load");
            fclose(file);
            return false;
        }

        char *token;
        int field_count = 0;

        token = strtok(line, ",\n");
        while (token != NULL) {
            // Kolom 0 -> ID
            if (field_count == 0) {
                new_doctor->id = atoi(token);
                // Kolom 1 -> Nama
            } else if (field_count == 1) {
                strncpy(new_doctor->name, token, MAX_NAME_LENGTH - 1);
                new_doctor->name[MAX_NAME_LENGTH - 1] = '\0';
                // Kolom 2 -> Jumlah shift maksimal per minggu
            } else if (field_count == 2) {
                new_doctor->max_shifts_per_week = atoi(token);
                // Kolom 3-7 -> Total shift yang terjadwal untuk minggu ke-n
                // Minggu ke-(0-4)
            } else if (field_count >= 3 && field_count < 3 + NUM_WEEKS) {
                new_doctor->shifts_scheduled_per_week[field_count - 3] =
                    atoi(token);
                // Kolom 8 Preferensi Hari dan Shift (0 = Tidak ingin)
            } else if (field_count >= 8 &&
                       field_count <
                           8 + (NUM_DAYS_PER_WEEK * NUM_SHIFTS_PER_DAY)) {
                int pref_index = field_count - 8;
                int day = pref_index / NUM_SHIFTS_PER_DAY;
                int shift = pref_index % NUM_SHIFTS_PER_DAY;
                new_doctor->preference[day][shift] = atoi(token);
            }

            token = strtok(NULL, ",\n");
            field_count++;
        }
        new_doctor->next = NULL;

        // Tambahkan dokter ke "end" dari list
        if (head == NULL) {
            head = new_doctor;
        } else {
            Doctor *current = head;
            while (current->next != NULL) {
                current = current->next;
            }
            current->next = new_doctor;
        }
    }

    fclose(file);
    refreshDoctorID(); // Refresh ID daftar dokter
    return true;
}

// Fungsi CSV untuk Scheduling

// Menyimpan Schedule ke CSV
bool save_schedule_to_csv(const char *filepath) {
    FILE *file = fopen(filepath, "w");
    if (file == NULL) {
        perror("Error opening file for writing");
        return false;
    }

    fprintf(file, "Week,Day,Morning,Afternoon,Night\n");

    for (int w = 0; w < NUM_WEEKS; w++) {
        for (int d = 0; d < NUM_DAYS_PER_WEEK; d++) {
            fprintf(file, "%d,%d,%d,%d,%d\n", w, d,
                    schedule[w][d][0], // Morning
                    schedule[w][d][1], // Afternoon
                    schedule[w][d][2]  // Night
            );
        }
    }

    fclose(file);
    return true;
}

// Load schedule dari CSV
bool load_schedule_from_csv(const char *filepath) {
    FILE *file = fopen(filepath, "r");

    // Check keberadaan file (ada atau tidak)
    if (file == NULL) {
        return false; // Tidak ada file, bukan error
    }

    initialize_schedule(); // Kosongkan schedule yang ada sebelum diload yang
                           // baru

    char line[256];
    // Skip line pertama (header)
    if (fgets(line, sizeof(line), file) == NULL) {
        fclose(file);
        return true; // Jika file kosong, bukan error
    }

    int w, d, m_id, a_id, n_id;
    while (fscanf(file, "%d,%d,%d,%d,%d\n", &w, &d, &m_id, &a_id, &n_id) == 5) {
        if (w >= 0 && w < NUM_WEEKS && d >= 0 && d < NUM_DAYS_PER_WEEK) {
            schedule[w][d][0] = m_id;
            schedule[w][d][1] = a_id;
            schedule[w][d][2] = n_id;
        }
    }

    fclose(file);
    return true;
}
