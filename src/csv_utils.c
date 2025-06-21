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
        perror("Gagal membuka file untuk penulisan");
        return false;
    }

    fprintf(file, "ID,Name,MaxShifts,Week1,Week2,Week3,Week4,Week5");
    for (int d = 0; d < NUM_DAYS_PER_WEEK; d++) {
        for (int s = 0; s < NUM_SHIFTS_PER_DAY; s++) {
            fprintf(file, ",Pref_D%dS%d", d, s);
        }
    }
    fprintf(file, "\n");

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
        return false;
    }

    freeDoctorList();

    char line[1024];
    if (fgets(line, sizeof(line), file) == NULL) {
        fclose(file);
        return true;
    }

    while (fgets(line, sizeof(line), file)) {
        if (strlen(line) < 2)
            continue;

        Doctor *new_doctor = (Doctor *)malloc(sizeof(Doctor));
        if (new_doctor == NULL) {
            perror("Alokasi memori gagal saat memuat CSV");
            fclose(file);
            return false;
        }
        new_doctor->next = NULL;

        char *token = strtok(line, ",\n");
        int field_count = 0;
        int total_prefs = NUM_DAYS_PER_WEEK * NUM_SHIFTS_PER_DAY;

        while (token != NULL) {
            switch (field_count) {
            case 0:
                new_doctor->id = atoi(token);
                break;
            case 1:
                strncpy(new_doctor->name, token, MAX_NAME_LENGTH - 1);
                new_doctor->name[MAX_NAME_LENGTH - 1] = '\0';
                break;
            case 2:
                new_doctor->max_shifts_per_week = atoi(token);
                break;
            default:
                if (field_count >= 3 && field_count < 3 + NUM_WEEKS) {
                    new_doctor->shifts_scheduled_per_week[field_count - 3] =
                        atoi(token);
                } else if (field_count >= 3 + NUM_WEEKS &&
                           field_count < 3 + NUM_WEEKS + total_prefs) {
                    int pref_index = field_count - (3 + NUM_WEEKS);
                    int day = pref_index / NUM_SHIFTS_PER_DAY;
                    int shift = pref_index % NUM_SHIFTS_PER_DAY;
                    if (day < NUM_DAYS_PER_WEEK && shift < NUM_SHIFTS_PER_DAY)
                        new_doctor->preference[day][shift] = atoi(token);
                }
                break;
            }
            token = strtok(NULL, ",\n");
            field_count++;
        }

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

// Fungsi CSV untuk Scheduling

// Menyimpan Schedule ke CSV
bool save_schedule_to_csv(const char *filepath) {
    FILE *file = fopen(filepath, "w");
    if (file == NULL) {
        perror("Gagal membuka file untuk penulisan");
        return false;
    }

    fprintf(file, "DayIndex,Morning,Afternoon,Night\n");

    for (int d = 0; d < NUM_DAYS; d++) {
        fprintf(file, "%d,%d,%d,%d\n", d, schedule[d][0], schedule[d][1],
                schedule[d][2]);
    }

    fclose(file);
    return true;
}

// Load schedule dari CSV
bool load_schedule_from_csv(const char *filepath) {
    FILE *file = fopen(filepath, "r");
    if (file == NULL) {
        return false;
    }

    initialize_schedule();

    char line[256];
    if (fgets(line, sizeof(line), file) == NULL) {
        fclose(file);
        return true;
    }

    int d, m_id, a_id, n_id;
    // FIX: Kondisi fscanf diubah dari '== 5' menjadi '== 4' agar sesuai format
    while (fscanf(file, "%d,%d,%d,%d", &d, &m_id, &a_id, &n_id) == 4) {
        if (d >= 0 && d < NUM_DAYS) {
            schedule[d][0] = m_id;
            schedule[d][1] = a_id;
            schedule[d][2] = n_id;
        }
    }

    fclose(file);
    return true;
}
