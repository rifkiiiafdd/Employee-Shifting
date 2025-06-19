#include "csv_utils.h"
#include "doctor_list.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#ifdef _WIN32
#include <direct.h>
#endif

void display_main_menu();
void handle_add_doctor();
void handle_edit_doctor();
void handle_remove_doctor();
void handle_set_preferences(int doctor_id);
void handle_load_data();
void handle_save_data();
bool create_directory_if_not_exists(const char *path);
void clear_input_buffer();
int get_integer_input();
void get_string_input(char *buffer, int size);

int main() {
    int choice = 0;
    initialize_schedule();
    printf("--- Doctor Scheduling CLI ---\n");

    while (choice != 7) {
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
            handle_load_data();
            break;
        case 6:
            handle_save_data();
            break;
        case 7:
            printf("Exiting program. Freeing memory...\n");
            break;
        default:
            printf("\n*** Invalid choice. Please enter a number between 1 and "
                   "7. ***\n");
            break;
        }
        if (choice != 7) {
            printf("\nPress Enter to continue...");
            clear_input_buffer();
        }
    }

    freeDoctorList();
    return 0;
}

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
    printf("5. Load Data from Directory\n");
    printf("6. Save Data to Directory\n");
    printf("7. Exit\n");
    printf("----------------------------------\n");
    printf("Enter your choice: ");
}

void handle_load_data() {
    char dir_path[256];
    char doc_path[512];
    char sched_path[512];

    printf("\n--- Load Data ---\n");
    printf("Enter the directory path to load data from (e.g., data): ");
    get_string_input(dir_path, sizeof(dir_path));

    snprintf(doc_path, sizeof(doc_path), "%s/doctors.csv", dir_path);
    snprintf(sched_path, sizeof(sched_path), "%s/schedule.csv", dir_path);

    printf("Loading doctors from %s...\n", doc_path);
    if (load_doctors_from_csv(doc_path)) {
        printf("Doctor list loaded successfully.\n");
    } else {
        printf("Could not load doctor list. The file may not exist yet.\n");
    }

    printf("Loading schedule from %s...\n", sched_path);
    if (load_schedule_from_csv(sched_path)) {
        printf("Schedule loaded successfully.\n");
    } else {
        printf("Could not load schedule. The file may not exist yet.\n");
    }
}

void handle_save_data() {
    char dir_path[256];
    char doc_path[512];
    char sched_path[512];

    printf("\n--- Save Data ---\n");
    printf("Enter the directory path to save data to (e.g., data): ");
    get_string_input(dir_path, sizeof(dir_path));

    if (!create_directory_if_not_exists(dir_path)) {
        return; // User cancelled or creation failed
    }

    snprintf(doc_path, sizeof(doc_path), "%s/doctors.csv", dir_path);
    snprintf(sched_path, sizeof(sched_path), "%s/schedule.csv", dir_path);

    printf("Saving doctors to %s...\n", doc_path);
    if (save_doctors_to_csv(doc_path)) {
        printf("Doctor list saved successfully.\n");
    } else {
        printf("Failed to save doctor list.\n");
    }

    printf("Saving schedule to %s...\n", sched_path);
    if (save_schedule_to_csv(sched_path)) {
        printf("Schedule saved successfully.\n");
    } else {
        printf("Failed to save schedule.\n");
    }
}

bool create_directory_if_not_exists(const char *path) {
    struct stat st = {0};

    // Check if directory exists
    if (stat(path, &st) == -1) {
        char choice;
        printf("Directory '%s' does not exist. Create it? (y/n): ", path);
        choice = getchar();
        clear_input_buffer();

        if (tolower(choice) == 'y') {
            int result;
#ifdef _WIN32
            result = _mkdir(path);
#else
            // mkdir with 0777 gives read/write/execute permissions to everyone
            result = mkdir(path, 0777);
#endif
            if (result == 0) {
                printf("Directory created successfully.\n");
                return true;
            } else {
                perror("Failed to create directory");
                return false;
            }
        } else {
            printf("Save operation cancelled.\n");
            return false;
        }
    }
    return true; // Directory already exists
}

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
        Doctor *new_doctor = findDoctorByName(name);
        if (new_doctor) {
            handle_set_preferences(new_doctor->id);
        }
    } else {
        printf("\n*** Invalid input. ***\n");
    }
}

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

        if (strlen(new_name) == 0) {
            strcpy(new_name, doctor->name);
        }

        updateDoctor(id, new_name, new_max_shifts);
        printf("Doctor information updated.\n");

        char edit_prefs_choice;
        printf("\nDo you want to edit the preferences for Dr. %s? (y/n): ",
               new_name);
        edit_prefs_choice = getchar();
        clear_input_buffer();

        if (tolower(edit_prefs_choice) == 'y') {
            handle_set_preferences(id);
        }
    } else {
        printf("\n*** Doctor with ID %d not found. ***\n", id);
    }
}

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

void handle_set_preferences(int doctor_id) {
    Doctor *doctor = findDoctorById(doctor_id);
    if (!doctor) {
        printf("\n*** Could not find doctor to set preferences. ***\n");
        return;
    }

    printf("\n--- Setting Preferences for Dr. %s ---\n", doctor->name);
    printf("Enter availability for each shift (1 for YES, 0 for NO).\n");
    printf("Enter -1 for the day to finish.\n\n");

    while (1) {
        int day, shift, can_do;
        printf("Enter Day (0=Sun, 1=Mon, ..., 6=Sat, -1=Done): ");
        day = get_integer_input();

        if (day == -1)
            break;

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

        printf("Is the doctor available? (1=Yes, 0=No): ");
        can_do = get_integer_input();

        if (can_do == 0 || can_do == 1) {
            setDoctorPreference(doctor_id, (DayOfWeek)day, (ShiftType)shift,
                                can_do);
        } else {
            printf("*** Invalid availability. ***\n");
        }
    }
}

void clear_input_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF)
        ;
}

int get_integer_input() {
    char buffer[100];
    if (fgets(buffer, sizeof(buffer), stdin) != NULL) {
        return atoi(buffer);
    }
    return -1;
}

void get_string_input(char *buffer, int size) {
    if (fgets(buffer, size, stdin) != NULL) {
        buffer[strcspn(buffer, "\n")] = '\0';
    }
}
