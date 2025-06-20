// TEMPORARY MAIN UNTUK CLI
// Fungsi Handler memang agak ribet, mau disimplifikasi lagi nanti

#include "csv_utils.h"
#include "doctor_list.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

// CLS/Clear screen utk spesifik OS
#ifdef _WIN32
#include <direct.h>
#define MKDIR(path) _mkdir(path)
#define CLEAR_SCREEN "cls"
#else
#define MKDIR(path) mkdir(path, 0777)
#define CLEAR_SCREEN "clear"
#endif

// Prototype fungsi
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
void run_cli_app();

int main(int argc, char *argv[]) {
    initialize_schedule();
    run_cli_app();

    // Free list dokter sebelum keluar
    freeDoctorList();
    printf("Memory freed. Goodbye!\n");
    return 0;
}

// Main loop CLI
void run_cli_app() {
    // Opsi menu
    const char *menu_options[] = {"Display All Doctors",
                                  "Add New Doctor",
                                  "Edit Doctor Information",
                                  "Remove Doctor",
                                  "Load Data from Directory",
                                  "Save Data to Directory",
                                  "Exit"};
    int num_options = sizeof(menu_options) / sizeof(menu_options[0]);

    // Array ke pointer fungsi???? How does this work bro
    void (*action_handlers[])() = {displayDoctors,     handle_add_doctor,
                                   handle_edit_doctor, handle_remove_doctor,
                                   handle_load_data,   handle_save_data};

    int choice = 0;
    while (choice != num_options) { // Loop sampai exit
        system(CLEAR_SCREEN);
        printf("\n--- Doctor Scheduler Main Menu ---\n");
        for (int i = 0; i < num_options; i++) {
            printf("%d. %s\n", i + 1, menu_options[i]);
        }
        printf("----------------------------------\n");
        printf("Enter your choice: ");

        choice = get_integer_input();

        // Validasi pilihan dan pemanggilan fungsi menggunakan array of pointer
        // to function
        if (choice > 0 && choice < num_options) {
            action_handlers[choice - 1]();
        } else if (choice == num_options) {
            printf("Exiting program...\n");
        } else {
            printf("\n*** Invalid choice. Please try again. ***\n");
        }

        if (choice != num_options) {
            printf("\nPress Enter to continue...");
            clear_input_buffer();
        }
    }
}

// Hanlder load data (dua-duanya)
void handle_load_data() {
    char dir_path[256];
    char doc_path[512];
    char sched_path[512];

    printf("\n--- Load Data ---\n");
    printf("Enter directory path to load from (e.g., data): ");
    get_string_input(dir_path, sizeof(dir_path));

    // Full path dari direktori dan nama file list dokter
    strcpy(doc_path, dir_path);
    strcat(doc_path, "/doctors.csv");

    // Full path dari direktori dan nama file schedule
    strcpy(sched_path, dir_path);
    strcat(sched_path, "/schedule.csv");

    printf("Loading doctors from '%s'...\n", doc_path);
    if (load_doctors_from_csv(doc_path)) {
        printf("Doctor list loaded successfully.\n");
    } else {
        printf("Could not load doctor list. The file may not exist.\n");
    }

    printf("Loading schedule from '%s'...\n", sched_path);
    if (load_schedule_from_csv(sched_path)) {
        printf("Schedule loaded successfully.\n");
    } else {
        printf("Could not load schedule. The file may not exist.\n");
    }
}

// Handler simpan data (dua-duanya)
void handle_save_data() {
    char dir_path[256];
    char doc_path[512];
    char sched_path[512];

    printf("\n--- Save Data ---\n");
    printf("Enter directory path to save to (e.g., data): ");
    get_string_input(dir_path, sizeof(dir_path));

    if (!create_directory_if_not_exists(dir_path)) {
        return;
    }

    // Full path dari direktori dan nama file list dokter
    strcpy(doc_path, dir_path);
    strcat(doc_path, "/doctors.csv");

    // Full path dari direktori dan nama file schedule
    strcpy(sched_path, dir_path);
    strcat(sched_path, "/schedule.csv");

    printf("Saving doctors to '%s'...\n", doc_path);
    if (save_doctors_to_csv(doc_path)) {
        printf("Doctor list saved successfully.\n");
    } else {
        printf("Failed to save doctor list.\n");
    }

    printf("Saving schedule to '%s'...\n", sched_path);
    if (save_schedule_to_csv(sched_path)) {
        printf("Schedule saved successfully.\n");
    } else {
        printf("Failed to save schedule.\n");
    }
}

bool create_directory_if_not_exists(const char *path) {
    struct stat st = {0};
    if (stat(path, &st) == -1) {
        printf("Directory '%s' does not exist. Create it? (y/n): ", path);
        int choice = getchar();
        clear_input_buffer();

        if (tolower(choice) == 'y') {
            if (MKDIR(path) == 0) {
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
    return true;
}

// Handler tambah data dokter
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
        printf("\n*** Invalid input. Name cannot be empty and shifts must be "
               "positive. ***\n");
    }
}

// Handler edit data dokter
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
        char new_name[MAX_NAME_LENGTH] = "";
        int new_max_shifts = -1;
        char buffer[100];

        printf("Editing Doctor ID %d: %s\n", id, doctor->name);
        printf("Enter new name (or press Enter to keep '%s'): ", doctor->name);
        get_string_input(new_name, sizeof(new_name));

        printf("Enter new max shifts per week (current: %d, press Enter to "
               "keep): ",
               doctor->max_shifts_per_week);
        get_string_input(buffer, sizeof(buffer));
        if (strlen(buffer) > 0) {
            new_max_shifts = atoi(buffer);
        }

        updateDoctor(id, new_name, new_max_shifts);
        printf("Doctor information updated.\n");

        printf("\nDo you want to edit preferences for Dr. %s? (y/n): ",
               doctor->name);
        int choice = getchar();
        clear_input_buffer();

        if (tolower(choice) == 'y') {
            handle_set_preferences(id);
        }
    } else {
        printf("\n*** Doctor with ID %d not found. ***\n", id);
    }
}

// Handler hapus dokter
void handle_remove_doctor() {
    if (!head) {
        printf("\n*** No doctors in the list to remove. ***\n");
        return;
    }

    printf("\n--- Remove Doctor ---\n");
    displayDoctors();
    printf("\nEnter the ID of the doctor to remove: ");
    int id = get_integer_input();

    Doctor *doctor = findDoctorById(id);
    if (doctor) {
        printf("Are you sure you want to remove Dr. %s (ID: %d)? (y/n): ",
               doctor->name, id);
        int confirmation = getchar();
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

// Handler preferensi dokter
void handle_set_preferences(int doctor_id) {
    Doctor *doctor = findDoctorById(doctor_id);
    if (!doctor) {
        printf("\n*** Could not find doctor to set preferences. ***\n");
        return;
    }

    printf("\n--- Setting Preferences for Dr. %s ---\n", doctor->name);
    printf("For each shift, enter 1 for AVAILABLE or 0 for UNAVAILABLE.\n");
    printf("Enter a day from 0-6 (Sun-Sat). Enter -1 to finish.\n\n");

    while (1) {
        int day, shift, can_do;
        printf("Enter Day (0-6) or -1 to Done: ");
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

        printf("Is Dr. %s available for this shift? (1=Yes, 0=No): ",
               doctor->name);
        can_do = get_integer_input();

        if (can_do == 0 || can_do == 1) {
            setDoctorPreference(doctor_id, day, shift, can_do);
        } else {
            printf("*** Invalid availability. Please enter 0 or 1. ***\n");
        }
    }
}

// Fungsi input
void clear_input_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF)
        ;
}

int get_integer_input() {
    char buffer[100];
    if (fgets(buffer, sizeof(buffer), stdin) != NULL) {
        if (strlen(buffer) <= 1 && (buffer[0] == '\n' || buffer[0] == '\0'))
            return -1;
        return atoi(buffer);
    }
    return -1;
}

void get_string_input(char *buffer, int size) {
    if (fgets(buffer, size, stdin) != NULL) {
        buffer[strcspn(buffer, "\n")] = '\0';
    }
}
