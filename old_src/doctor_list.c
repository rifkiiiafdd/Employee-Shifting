#include "doctor_list.h" // Include the header file for declarations
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Global head of the linked list (defined here, declared extern in header)
Doctor *head = NULL;
// Global variable for auto-incrementing ID for new doctors.
// It's static to this file as it's an implementation detail.
static int next_doctor_id = 1;

/**
 * @brief Creates a new Doctor node.
 *
 * @param name The name of the doctor.
 * @param max_shifts The maximum shifts per week.
 * @return A pointer to the newly created Doctor node, or NULL if memory
 * allocation fails.
 */
Doctor *createDoctor(const char *name, int max_shifts) {
    Doctor *newDoctor = (Doctor *)malloc(sizeof(Doctor));
    if (newDoctor == NULL) {
        printf("Memory allocation failed for new doctor.\n");
        return NULL;
    }

    newDoctor->id = next_doctor_id++;
    strncpy(newDoctor->name, name, MAX_NAME_LENGTH - 1);
    newDoctor->name[MAX_NAME_LENGTH - 1] = '\0'; // Ensure null-termination
    newDoctor->max_shifts_per_week = max_shifts;

    // Initialize shifts scheduled per week to 0 for all weeks
    for (int i = 0; i < NUM_WEEKS; i++) {
        newDoctor->shifts_scheduled_per_week[i] = 0;
    }

    // Initialize all preferences to 1 (can do all shifts by default)
    for (int i = 0; i < NUM_DAYS_PER_WEEK; i++) {
        for (int j = 0; j < NUM_SHIFTS_PER_DAY; j++) {
            newDoctor->preference[i][j] = 1; // Default: can do the shift
        }
    }

    newDoctor->next = NULL;
    return newDoctor;
}

/**
 * @brief Adds a new doctor to the linked list.
 *
 * @param name The name of the doctor.
 * @param max_shifts The maximum shifts per week.
 */
void addDoctor(const char *name, int max_shifts) {
    Doctor *newDoctor = createDoctor(name, max_shifts);
    if (newDoctor == NULL) {
        return; // Failed to create doctor
    }

    if (head == NULL) {
        head = newDoctor; // If list is empty, new doctor is the head
    } else {
        Doctor *current = head;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = newDoctor; // Add to the end of the list
    }
    // No need to print ID here, as refreshDoctorIDs will re-sequence them.
    // If you add doctors one by one and want to see the ID, you can uncomment.
    // printf("Doctor '%s' (ID: %d) added successfully.\n", newDoctor->name,
    // newDoctor->id);
}

/**
 * @brief Refreshes the IDs of all doctors in the linked list to be sequential
 * starting from 1. This function also updates the global next_doctor_id to
 * reflect the new highest ID + 1.
 */
void refreshDoctorIDs() {
    if (head == NULL) {
        next_doctor_id = 1; // Reset if list is empty
        return;
    }

    Doctor *current = head;
    int current_id = 1;
    while (current != NULL) {
        current->id = current_id++;
        current = current->next;
    }
    next_doctor_id = current_id; // Set next_doctor_id for future additions
    printf("Doctor IDs refreshed successfully. Next available ID: %d\n",
           next_doctor_id);
}

/**
 * @brief Removes a doctor from the linked list by ID.
 * After removal, it calls refreshDoctorIDs to re-sequence the IDs.
 *
 * @param id The ID of the doctor to remove.
 */
void removeDoctor(int id) {
    Doctor *current = head;
    Doctor *prev = NULL;
    bool found = false;

    // Traverse the list to find the doctor with the given ID
    while (current != NULL && current->id != id) {
        prev = current;
        current = current->next;
    }

    if (current == NULL) {
        printf("Doctor with ID %d not found.\n", id);
        return;
    }

    if (prev == NULL) {
        head = current->next; // Doctor to remove is the head
    } else {
        prev->next = current->next; // Bypass the doctor to remove
    }

    printf("Doctor '%s' (ID: %d) removed successfully.\n", current->name,
           current->id);
    free(current); // Free the memory of the removed doctor
    found = true;

    if (found) {
        refreshDoctorIDs(); // Re-sequence IDs after removal
    }
}

/**
 * @brief Displays all doctors in the linked list.
 */
void displayDoctors() {
    if (head == NULL) {
        printf("No doctors in the list.\n");
        return;
    }

    printf("\n--- Doctor List ---\n");
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

/**
 * @brief Sets a specific preference for a doctor (0 for cannot, 1 for can).
 * This implements the "black list" system where you set 0 for unavailable.
 *
 * @param doctor_id The ID of the doctor to update.
 * @param day The day of the week (0=Sunday, 1=Monday, ..., 6=Saturday).
 * @param shift The shift type (0=Morning, 1=Afternoon, 2=Night).
 * @param can_do A boolean value (0 or 1) indicating if the doctor can do the
 * shift.
 */
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

/**
 * @brief Frees all memory allocated for the linked list.
 */
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
