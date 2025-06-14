#include "doctor_list.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// --- Forward Declarations for Menu and Helper Functions ---
void display_main_menu();
void handle_add_doctor();
void handle_edit_doctor();
void handle_remove_doctor();
void handle_set_preferences(int doctor_id); // Now takes an ID
void clear_input_buffer();
int get_integer_input();
void get_string_input(char *buffer, int size);

int main() {
    int choice = 0;
    printf("--- Doctor Scheduling CLI ---\n");

    while (choice != 5) {
        display_main_menu();
        choice = get_integer_input();

        switch (choice) {
        case 1:
            displayDoctors();
            break;
        case 2:
            handle_add_doctor();
            break;
        case 3:
            handle_edit_doctor();
            break;
        case 4:
            handle_remove_doctor();
            break;
        case 5:
            printf("Exiting program. Freeing memory...\n");
            break; // Exit the loop
        default:
            printf("\n*** Invalid choice. Please enter a number between 1 and "
                   "5. ***\n");
            break;
        }
        // Pause for user to see output before clearing screen
        if (choice != 5) {
            printf("\nPress Enter to continue...");
            clear_input_buffer();
        }
    }

    freeDoctorList();
    return 0;
}

/**
 * @brief Displays the main menu options to the console.
 */
void display_main_menu() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
    printf("\n--- Doctor Scheduler Main Menu ---\n");
    printf("1. Display All Doctors\n");
    printf("2. Add New Doctor\n");
    printf("3. Edit Doctor Information\n");
    printf("4. Remove Doctor\n");
    printf("5. Exit\n");
    printf("----------------------------------\n");
    printf("Enter your choice: ");
}

/**
 * @brief Handles the user workflow for adding a new doctor and setting their
 * initial preferences.
 */
void handle_add_doctor() {
    char name[MAX_NAME_LENGTH];
    int max_shifts;

    printf("\n--- Add New Doctor ---\n");
    printf("Enter doctor's name: ");
    get_string_input(name, sizeof(name));

    printf("Enter max shifts per week: ");
    max_shifts = get_integer_input();

    if (strlen(name) > 0 && max_shifts > 0) {
        addDoctor(name, max_shifts);
        // Automatically proceed to set preferences for the new doctor
        Doctor *new_doctor = findDoctorByName(name);
        if (new_doctor) {
            handle_set_preferences(new_doctor->id);
        }
    } else {
        printf("\n*** Invalid input. Name cannot be empty and max shifts must "
               "be positive. ***\n");
    }
}

/**
 * @brief Handles the user workflow for editing an existing doctor's details,
 * including their shift preferences.
 */
void handle_edit_doctor() {
    if (!head) {
        printf("\n*** No doctors in the list to edit. ***\n");
        return;
    }
    printf("\n--- Edit Doctor Information ---\n");
    displayDoctors();
    printf("\nEnter the ID of the doctor to edit: ");
    int id = get_integer_input();

    Doctor *doctor = findDoctorById(id);
    if (doctor) {
        char new_name[MAX_NAME_LENGTH];
        int new_max_shifts;

        printf("Editing Doctor ID %d: %s\n", id, doctor->name);
        printf("Enter new name (or press Enter to keep '%s'): ", doctor->name);
        get_string_input(new_name, sizeof(new_name));

        printf("Enter new max shifts per week (current is %d): ",
               doctor->max_shifts_per_week);
        new_max_shifts = get_integer_input();

        // If the user just pressed enter for the name, keep the old name
        if (strlen(new_name) == 0) {
            strcpy(new_name, doctor->name);
        }

        updateDoctor(id, new_name, new_max_shifts);
        printf("Doctor information updated.\n");

        // --- NEW: Ask to edit preferences ---
        char edit_prefs_choice;
        printf("\nDo you want to edit the preferences for Dr. %s? (y/n): ",
               new_name);
        edit_prefs_choice = getchar();
        clear_input_buffer(); // Clear buffer after reading a single character

        if (tolower(edit_prefs_choice) == 'y') {
            handle_set_preferences(id);
        }
        // --- End of new feature ---

    } else {
        printf("\n*** Doctor with ID %d not found. ***\n", id);
    }
}

/**
 * @brief Handles the user workflow for removing a doctor from the list.
 */
void handle_remove_doctor() {
    if (!head) {
        printf("\n*** No doctors in the list to remove. ***\n");
        return;
    }
    printf("\n--- Remove Doctor ---\n");
    displayDoctors();
    printf("\nEnter the ID of the doctor to remove: ");
    int id = get_integer_input();

    Doctor *doctor_to_remove = findDoctorById(id);
    if (doctor_to_remove) {
        char confirmation;
        printf("Are you sure you want to remove Dr. %s (ID: %d)? (y/n): ",
               doctor_to_remove->name, id);
        confirmation = getchar();
        clear_input_buffer();

        if (tolower(confirmation) == 'y') {
            removeDoctor(id);
        } else {
            printf("Removal cancelled.\n");
        }
    } else {
        printf("\n*** Doctor with ID %d not found. ***\n", id);
    }
}

/**
 * @brief Manages the interactive sub-menu for setting a doctor's shift
 * preferences.
 * @param doctor_id The ID of the doctor whose preferences are being set.
 */
void handle_set_preferences(int doctor_id) {
    Doctor *doctor = findDoctorById(doctor_id);
    if (!doctor) {
        printf("\n*** Could not find doctor to set preferences. ***\n");
        return;
    }

    printf("\n--- Setting Preferences for Dr. %s ---\n", doctor->name);
    printf("Enter availability for each shift (1 for YES, 0 for NO).\n");
    printf("You can enter preferences for multiple shifts.\n");
    printf("Enter -1 for the day to finish.\n\n");

    while (1) {
        int day, shift, can_do;
        printf("Enter Day (0=Sun, 1=Mon, ..., 6=Sat, -1=Done): ");
        day = get_integer_input();

        if (day == -1) {
            break;
        }

        if (day < 0 || day >= NUM_DAYS_PER_WEEK) {
            printf("*** Invalid day. Please try again. ***\n");
            continue;
        }

        printf("Enter Shift (0=Morning, 1=Afternoon, 2=Night): ");
        shift = get_integer_input();

        if (shift < 0 || shift >= NUM_SHIFTS_PER_DAY) {
            printf("*** Invalid shift. Please try again. ***\n");
            continue;
        }

        printf("Is the doctor available for this shift? (1=Yes, 0=No): ");
        can_do = get_integer_input();

        if (can_do == 0 || can_do == 1) {
            setDoctorPreference(doctor_id, (DayOfWeek)day, (ShiftType)shift,
                                can_do);
        } else {
            printf("*** Invalid availability. Please enter 0 or 1. ***\n");
        }
    }
    printf("\n--- Preferences for Dr. %s have been set. ---\n", doctor->name);
}

/**
 * @brief Clears the standard input buffer of any remaining characters.
 * This is crucial after reading a number or char to consume the leftover
 * newline.
 */
void clear_input_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF)
        ;
}

/**
 * @brief Safely reads an integer from standard input.
 * @return The integer value entered by the user.
 */
int get_integer_input() {
    char buffer[100];
    if (fgets(buffer, sizeof(buffer), stdin) != NULL) {
        return atoi(buffer);
    }
    return -1; // Return an error code if input fails
}

/**
 * @brief Safely reads a line of text from standard input.
 * @param buffer The buffer to store the read string.
 * @param size The size of the buffer.
 */
void get_string_input(char *buffer, int size) {
    if (fgets(buffer, size, stdin) != NULL) {
        // Remove trailing newline character
        buffer[strcspn(buffer, "\n")] = '\0';
    }
}
