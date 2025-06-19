#include "doctor_list.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// --- Global Variable Definitions ---
Doctor *head = NULL;
int next_doctor_id = 1;
int schedule[NUM_WEEKS][NUM_DAYS_PER_WEEK][NUM_SHIFTS_PER_DAY];

void initialize_schedule() {
    for (int w = 0; w < NUM_WEEKS; w++) {
        for (int d = 0; d < NUM_DAYS_PER_WEEK; d++) {
            for (int s = 0; s < NUM_SHIFTS_PER_DAY; s++) {
                schedule[w][d][s] = 0;
            }
        }
    }
}

Doctor *createDoctor(const char *name, int max_shifts_per_week) {
    Doctor *newDoctor = (Doctor *)malloc(sizeof(Doctor));
    if (newDoctor == NULL) {
        printf("Memory allocation failed for new doctor.\n");
        return NULL;
    }

    newDoctor->id = next_doctor_id++;
    strncpy(newDoctor->name, name, MAX_NAME_LENGTH - 1);
    newDoctor->name[MAX_NAME_LENGTH - 1] = '\0';
    newDoctor->max_shifts_per_week = max_shifts_per_week;

    for (int i = 0; i < NUM_WEEKS; i++) {
        newDoctor->shifts_scheduled_per_week[i] = 0;
    }

    for (int i = 0; i < NUM_DAYS_PER_WEEK; i++) {
        for (int j = 0; j < NUM_SHIFTS_PER_DAY; j++) {
            newDoctor->preference[i][j] = 1;
        }
    }

    newDoctor->next = NULL;
    return newDoctor;
}

void addDoctor(const char *name, int max_shifts_per_week) {
    Doctor *newDoctor = createDoctor(name, max_shifts_per_week);
    if (newDoctor == NULL) {
        return;
    }

    if (head == NULL) {
        head = newDoctor;
    } else {
        Doctor *current = head;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = newDoctor;
    }
    printf("Doctor '%s' (ID: %d) added successfully.\n", newDoctor->name,
           newDoctor->id);
}

void refreshDoctorID() {
    if (head == NULL) {
        next_doctor_id = 1;
        return;
    }

    int max_id = 0;
    Doctor *current = head;
    while (current != NULL) {
        if (current->id > max_id) {
            max_id = current->id;
        }
        current = current->next;
    }
    next_doctor_id = max_id + 1;
    printf("Doctor ID system refreshed. Next available ID: %d\n",
           next_doctor_id);
}

void removeDoctor(int id) {
    Doctor *current = head;
    Doctor *prev = NULL;

    while (current != NULL && current->id != id) {
        prev = current;
        current = current->next;
    }

    if (current == NULL) {
        printf("Doctor (ID: %d) not found.\n", id);
        return;
    }

    if (prev == NULL) {
        head = current->next;
    } else {
        prev->next = current->next;
    }

    printf("Doctor '%s' (ID: %d) removed successfully.\n", current->name, id);
    free(current);
    refreshDoctorID();
}

void displayDoctors() {
    if (head == NULL) {
        printf("\n*** No doctors in the list. ***\n");
        return;
    }

    printf("\n--- Doctor List ---\n");
    printf("%-5s %-25s %-15s\n", "ID", "Name", "Max Shifts/Wk");
    printf("--------------------------------------------------\n");
    Doctor *current = head;
    while (current != NULL) {
        printf("%-5d %-25s %-15d\n", current->id, current->name,
               current->max_shifts_per_week);
        current = current->next;
    }
    printf("--------------------------------------------------\n");
}

void setDoctorPreference(int doctor_id, DayOfWeek day, ShiftType shift,
                         int can_do) {
    if (day < 0 || day >= NUM_DAYS_PER_WEEK || shift < 0 ||
        shift >= NUM_SHIFTS_PER_DAY) {
        printf("Invalid day or shift type provided.\n");
        return;
    }

    Doctor *current = findDoctorById(doctor_id);
    if (current == NULL) {
        printf("Doctor with ID %d not found to set preference.\n", doctor_id);
        return;
    }

    current->preference[day][shift] = can_do;
    printf("Preference for Doctor %s updated.\n", current->name);
}

void freeDoctorList() {
    Doctor *current = head;
    Doctor *next_node;
    while (current != NULL) {
        next_node = current->next;
        free(current);
        current = next_node;
    }
    head = NULL;
    printf("All doctor data freed from memory.\n");
}

Doctor *findDoctorById(int id) {
    Doctor *current = head;
    while (current != NULL) {
        if (current->id == id) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

Doctor *findDoctorByName(const char *name) {
    Doctor *current = head;
    while (current != NULL) {
        if (strcmp(current->name, name) == 0) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

void updateDoctor(int id, const char *new_name, int new_max_shifts) {
    Doctor *doctor_to_update = findDoctorById(id);
    if (doctor_to_update != NULL) {
        strncpy(doctor_to_update->name, new_name, MAX_NAME_LENGTH - 1);
        doctor_to_update->name[MAX_NAME_LENGTH - 1] = '\0';

        if (new_max_shifts > 0) {
            doctor_to_update->max_shifts_per_week = new_max_shifts;
        } else {
            printf(
                "Warning: Invalid max shifts value (%d). Retaining original.\n",
                new_max_shifts);
        }
    } else {
        printf("Error: Could not find doctor with ID %d to update.\n", id);
    }
}
