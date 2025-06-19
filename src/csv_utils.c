#include "csv_utils.h"
#include "doctor_list.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

bool save_doctors_to_csv(const char *filepath) {
    FILE *file = fopen(filepath, "w");
    if (file == NULL) {
        perror("Error opening file for writing");
        return false;
    }

    // Write header
    fprintf(file, "ID,Name,MaxShifts,Week1,Week2,Week3,Week4,Week5");
    for (int d = 0; d < NUM_DAYS_PER_WEEK; d++) {
        for (int s = 0; s < NUM_SHIFTS_PER_DAY; s++) {
            fprintf(file, ",Pref_D%dS%d", d, s);
        }
    }
    fprintf(file, "\n");

    // Write data rows
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

bool load_doctors_from_csv(const char *filepath) {
    FILE *file = fopen(filepath, "r");
    if (file == NULL) {
        return false;
    }

    freeDoctorList();

    char line[1024];
    // Cek isi file, sekalian skip 1 line kalau berisi
    if (fgets(line, sizeof(line), file) == NULL) {
        fclose(file);
        return true; // Empty file is not an error
    }

    while (fgets(line, sizeof(line), file)) {
        if (strlen(line) < 2)
            continue;

        Doctor *new_doctor = (Doctor *)malloc(sizeof(Doctor));
        if (!new_doctor) {
            printf("Memory allocation failed during CSV load.\n");
            fclose(file);
            return false;
        }

        char *token;
        int field_count = 0;

        // Use a copy for strtok as it modifies the string
        char line_copy[1024];
        strncpy(line_copy, line, sizeof(line_copy));

        token = strtok(line_copy, ",");
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
            // Workload fields (3 to 7)
            case 3:
            case 4:
            case 5:
            case 6:
            case 7:
                new_doctor->shifts_scheduled_per_week[field_count - 3] =
                    atoi(token);
                break;
            // Preference fields (8 to 28)
            default:
                if (field_count >= 8 &&
                    field_count <
                        8 + (NUM_DAYS_PER_WEEK * NUM_SHIFTS_PER_DAY)) {
                    int pref_index = field_count - 8;
                    int day = pref_index / NUM_SHIFTS_PER_DAY;
                    int shift = pref_index % NUM_SHIFTS_PER_DAY;
                    new_doctor->preference[day][shift] = atoi(token);
                }
                break;
            }
            token = strtok(NULL, ",");
            field_count++;
        }
        new_doctor->next = NULL;

        // Add the new doctor to the end of the list
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
    refreshDoctorID(); // Update the next_doctor_id to avoid conflicts
    return true;
}

// --- Schedule CSV Functions ---

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

bool load_schedule_from_csv(const char *filepath) {
    FILE *file = fopen(filepath, "r");
    if (file == NULL) {
        return false; // File doesn't exist, not an error
    }

    initialize_schedule(); // Clear existing schedule

    char line[256];
    // Skip header
    if (fgets(line, sizeof(line), file) == NULL) {
        fclose(file);
        return true; // Empty file
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
