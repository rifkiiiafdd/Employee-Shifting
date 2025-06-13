#include "doctor_list.h"
#include <string.h>

Doctor *head = NULL;
int next_doctor_id = 1;

Doctor *createDoctor(const char *name, int max_shifts_per_week) {
    Doctor *newDoctor = (Doctor *)malloc(sizeof(Doctor));
    if (newDoctor == NULL) {
        printf("Memorry allocation failed for new doctor.\n");
        return NULL;
    }

    newDoctor->id = next_doctor_id++;
    strncpy(newDoctor->name, name, MAX_NAME_LENGTH);
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

    Doctor *current = head;
    int current_id = 1;
    while (current != NULL) {
        current->id = current_id++;
        current = current->next;
    }
    printf("Doctor IDs refreshed successfully.\n");
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

    printf("Doctor '%s' (ID: %d) remove successfully.\n", current->name, id);
    free(current);
    refreshDoctorID();
}

void displayDoctors() {
    if (head == NULL) {
        printf("No doctors in the list.\n");
        return;
    }

    printf("\n--- Doctor List (Terminal Output) ---\n");
    Doctor *current = head;
    while (current != NULL) {
        printf("ID: %d, Name: %s, Max Shifts/Week: %d\n", current->id,
               current->name, current->max_shifts_per_week);

        // Display shifts scheduled per week
        printf("  Shifts Scheduled (per week): ");
        for (int i = 0; i < NUM_WEEKS; i++) {
            printf("Week %d: %d ", i + 1,
                   current->shifts_scheduled_per_week[i]);
        }
        printf("\n");

        // Display preferences
        printf("  Preferences (0=cannot, 1=can):\n");
        const char *days[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
        const char *shifts[] = {"Morning", "Afternoon", "Night"};
        for (int i = 0; i < NUM_DAYS_PER_WEEK; i++) {
            printf("    %s: ", days[i]);
            for (int j = 0; j < NUM_SHIFTS_PER_DAY; j++) {
                printf("%s: %d ", shifts[j], current->preference[i][j]);
            }
            printf("\n");
        }
        printf("---------------------\n");
        current = current->next;
    }
}

void setDoctorPreference(int doctor_id, DayOfWeek day, ShiftType shift,
                         int can_do) {
    if (day < 0 || day >= NUM_DAYS_PER_WEEK || shift < 0 ||
        shift >= NUM_SHIFTS_PER_DAY) {
        printf("Invalid day or shift type provided.\n");
        return;
    }

    Doctor *current = head;
    while (current != NULL && current->id != doctor_id) {
        current = current->next;
    }

    if (current == NULL) {
        printf("Doctor with ID %d not found to set preference.\n", doctor_id);
        return;
    }

    current->preference[day][shift] = can_do;
    printf("Preference for Doctor %s (ID: %d) on %s %s set to %d.\n",
           current->name, current->id,
           (char *[]){"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday",
                      "Friday", "Saturday"}[day],
           (char *[]){"Morning", "Afternoon", "Night"}[shift], can_do);
}

void freeDoctorList() {
    Doctor *current = head;
    Doctor *next_node;
    while (current != NULL) {
        next_node = current->next;
        free(current);
        current = next_node;
    }
    head = NULL; // Reset head to NULL after freeing all nodes
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
    return NULL; // Doctor not found
}
