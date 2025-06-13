#include "csv_utils.h"
#include "doctor_list.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Function prototypes for menu functions
void showMainMenu();
void handleAddDoctor();
void handleEditDoctor();
void handleDeleteDoctor();
void handleDisplayDoctors();
void handleLoadFromCSV();
void handleSaveToCSV();
void handleSetPreferences();
void handleShowDoctorDetails();
void handleSetPreferencesForDoctor(int doctor_id);
void handleIndividualPreferences(int doctor_id);
void handleBulkPreferences(int doctor_id);
void resetAllPreferences(int doctor_id, int value);
void clearInputBuffer();
int getIntInput();
char *getStringInput(char *buffer, int size);

int main() {
    int choice;

    printf("=== DOCTOR SCHEDULING SYSTEM ===\n");
    printf("Hospital Management System - Doctor Shift Scheduling\n\n");

    // Automatically try to load a default file on startup
    printf("Attempting to load 'data/doctors.csv' on startup...\n");
    loadDoctorsFromCSV("data/doctors.csv");
    printf("\nPress Enter to continue...");
    getchar();

    while (1) {
        showMainMenu();
        printf("Enter your choice: ");
        choice = getIntInput();

        switch (choice) {
        case 1:
            handleLoadFromCSV();
            break;
        case 2:
            handleAddDoctor();
            break;
        case 3:
            handleEditDoctor();
            break;
        case 4:
            handleDeleteDoctor();
            break;
        case 5:
            handleDisplayDoctors();
            break;
        case 6:
            handleShowDoctorDetails();
            break;
        case 7:
            handleSetPreferences();
            break;
        case 8:
            handleSaveToCSV();
            break;
        case 9:
            printf("Exiting program...\n");
            freeDoctorList();
            exit(0);
        default:
            printf("Invalid choice. Please try again.\n");
        }

        printf("\nPress Enter to continue...");
        getchar();
    }

    return 0;
}

void showMainMenu() {
// Clear screen for better readability - works on Linux/macOS/Windows
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif

    printf("\n=== MAIN MENU ===\n");
    printf("1. Load doctors from CSV file\n");
    printf("2. Add new doctor\n");
    printf("3. Edit doctor information\n");
    printf("4. Delete doctor\n");
    printf("5. Display all doctors\n");
    printf("6. Show doctor details\n");
    printf("7. Set doctor preferences\n");
    printf("8. Save doctors to CSV file\n");
    printf("9. Exit\n");
    printf("==================\n");
}

void handleLoadFromCSV() {
    char user_filename[256];
    char full_path[512];

    printf("\nEnter CSV filename from the 'data/' directory (e.g., "
           "doctors.csv): ");
    getStringInput(user_filename, sizeof(user_filename));

    if (strlen(user_filename) == 0) {
        printf("No filename entered. Operation cancelled.\n");
        return;
    }

    // Construct the full path relative to the project root
    snprintf(full_path, sizeof(full_path), "data/%s", user_filename);

    printf("Attempting to load from '%s'...\n", full_path);
    loadDoctorsFromCSV(full_path);
}

void handleSaveToCSV() {
    char user_filename[256];
    char full_path[512];

    if (head == NULL) {
        printf("No doctors to save.\n");
        return;
    }

    printf("\nEnter CSV filename to save in the 'data/' directory (e.g., "
           "doctors_saved.csv): ");
    getStringInput(user_filename, sizeof(user_filename));

    if (strlen(user_filename) == 0) {
        printf("No filename entered. Operation cancelled.\n");
        return;
    }

    // Construct the full path relative to the project root
    snprintf(full_path, sizeof(full_path), "data/%s", user_filename);

    printf("Attempting to save to '%s'...\n", full_path);
    saveDoctorsToCSV(full_path);
}

void handleAddDoctor() {
    char name[MAX_NAME_LENGTH];
    int max_shifts;

    printf("\n=== ADD NEW DOCTOR ===\n");
    printf("Enter doctor name: ");
    getStringInput(name, sizeof(name));

    if (strlen(name) == 0) {
        printf("Name cannot be empty.\n");
        return;
    }

    // Check if doctor already exists
    if (findDoctorByName(name) != NULL) {
        printf("Doctor with name '%s' already exists.\n", name);
        return;
    }

    printf("Enter maximum shifts per week: ");
    max_shifts = getIntInput();

    if (max_shifts <= 0 || max_shifts > 21) { // Max 3 shifts per day * 7 days
        printf("Invalid number of shifts. Must be between 1 and 21.\n");
        return;
    }

    addDoctor(name, max_shifts);

    // Ask if user wants to set preferences now
    printf("\nDo you want to set shift preferences now? (y/n): ");
    char response = getchar();
    clearInputBuffer();

    if (response == 'y' || response == 'Y') {
        Doctor *doctor = findDoctorByName(name);
        if (doctor != NULL) {
            handleSetPreferencesForDoctor(doctor->id);
        }
    }
}

void handleEditDoctor() {
    int id;
    char name[MAX_NAME_LENGTH];
    int max_shifts;

    if (head == NULL) {
        printf("No doctors to edit.\n");
        return;
    }

    printf("\n=== EDIT DOCTOR ===\n");
    displayDoctors();
    printf("Enter doctor ID to edit: ");
    id = getIntInput();

    Doctor *doctor = findDoctorById(id);
    if (doctor == NULL) {
        printf("Doctor with ID %d not found.\n", id);
        return;
    }

    printf("Current name: %s\n", doctor->name);
    printf("Enter new name (or press Enter to keep current): ");
    getStringInput(name, sizeof(name));

    if (strlen(name) == 0) {
        strcpy(name, doctor->name); // Keep current name
    }

    printf("Current max shifts per week: %d\n", doctor->max_shifts_per_week);
    printf("Enter new max shifts per week: ");
    max_shifts = getIntInput();

    if (max_shifts <= 0 || max_shifts > 21) {
        printf("Invalid number of shifts. Keeping current value.\n");
        max_shifts = doctor->max_shifts_per_week;
    }

    updateDoctor(id, name, max_shifts);
}

void handleDeleteDoctor() {
    int id;

    if (head == NULL) {
        printf("No doctors to delete.\n");
        return;
    }

    printf("\n=== DELETE DOCTOR ===\n");
    displayDoctors();
    printf("Enter doctor ID to delete: ");
    id = getIntInput();

    Doctor *doctor = findDoctorById(id);
    if (doctor == NULL) {
        printf("Doctor with ID %d not found.\n", id);
        return;
    }

    printf("Are you sure you want to delete Dr. %s? (y/n): ", doctor->name);
    char response = getchar();
    clearInputBuffer();

    if (response == 'y' || response == 'Y') {
        removeDoctor(id);
    } else {
        printf("Delete operation cancelled.\n");
    }
}

void handleDisplayDoctors() {
    printf("\n");
    displayDoctors();
}

void handleShowDoctorDetails() {
    int id;

    if (head == NULL) {
        printf("No doctors registered.\n");
        return;
    }

    printf("\n=== DOCTOR DETAILS ===\n");
    displayDoctors();
    printf("Enter doctor ID to view details: ");
    id = getIntInput();

    displayDoctorDetails(id);
}

void handleSetPreferences() {
    int id;

    if (head == NULL) {
        printf("No doctors registered.\n");
        return;
    }

    printf("\n=== SET DOCTOR PREFERENCES ===\n");
    displayDoctors();
    printf("Enter doctor ID to set preferences: ");
    id = getIntInput();

    handleSetPreferencesForDoctor(id);
}

void handleSetPreferencesForDoctor(int doctor_id) {
    Doctor *doctor = findDoctorById(doctor_id);
    if (doctor == NULL) {
        printf("Doctor with ID %d not found.\n", doctor_id);
        return;
    }

    int choice;
    printf("\n=== SET PREFERENCES FOR DR. %s ===\n", doctor->name);
    printf("1. Set preferences individually\n");
    printf("2. Set all preferences at once\n");
    printf("3. Reset all to available\n");
    printf("4. Reset all to unavailable\n");
    printf("Enter your choice: ");
    choice = getIntInput();

    switch (choice) {
    case 1:
        handleIndividualPreferences(doctor_id);
        break;
    case 2:
        handleBulkPreferences(doctor_id);
        break;
    case 3:
        resetAllPreferences(doctor_id, 1);
        break;
    case 4:
        resetAllPreferences(doctor_id, 0);
        break;
    default:
        printf("Invalid choice.\n");
    }
}

void handleIndividualPreferences(int doctor_id) {
    Doctor *doctor = findDoctorById(doctor_id);
    if (doctor == NULL)
        return;

    printf("\nSetting individual preferences for Dr. %s\n", doctor->name);
    printf("Days: 0=Sunday, 1=Monday, 2=Tuesday, 3=Wednesday, 4=Thursday, "
           "5=Friday, 6=Saturday\n");
    printf("Shifts: 0=Morning, 1=Afternoon, 2=Night\n");
    printf("Availability: 0=Cannot do, 1=Can do\n\n");

    while (1) {
        int day, shift, availability;

        printf("Enter day (0-6, -1 to finish): ");
        day = getIntInput();

        if (day == -1)
            break;

        if (day < 0 || day >= NUM_DAYS_PER_WEEK) {
            printf("Invalid day. Please enter 0-6.\n");
            continue;
        }

        printf("Enter shift (0-2): ");
        shift = getIntInput();

        if (shift < 0 || shift >= NUM_SHIFTS_PER_DAY) {
            printf("Invalid shift. Please enter 0-2.\n");
            continue;
        }

        printf("Current availability: %s\n",
               doctor->preference[day][shift] ? "Available" : "Not Available");
        printf("Enter new availability (0 or 1): ");
        availability = getIntInput();

        if (availability != 0 && availability != 1) {
            printf("Invalid availability. Please enter 0 or 1.\n");
            continue;
        }

        setDoctorPreference(doctor_id, (DayOfWeek)day, (ShiftType)shift,
                            availability);
    }
}

void handleBulkPreferences(int doctor_id) {
    Doctor *doctor = findDoctorById(doctor_id);
    if (doctor == NULL)
        return;

    int preferences[NUM_DAYS_PER_WEEK][NUM_SHIFTS_PER_DAY];

    printf("\nSetting bulk preferences for Dr. %s\n", doctor->name);
    printf("Enter availability for each day-shift combination (0=Cannot do, "
           "1=Can do):\n\n");

    for (int day = 0; day < NUM_DAYS_PER_WEEK; day++) {
        printf("%s:\n", getDayString((DayOfWeek)day));
        for (int shift = 0; shift < NUM_SHIFTS_PER_DAY; shift++) {
            printf(
                "  %s shift (current: %s): ", getShiftString((ShiftType)shift),
                doctor->preference[day][shift] ? "Available" : "Not Available");
            int value = getIntInput();
            preferences[day][shift] = (value != 0) ? 1 : 0;
        }
        printf("\n");
    }

    setAllDoctorPreferences(doctor_id, preferences);
}

void resetAllPreferences(int doctor_id, int value) {
    Doctor *doctor = findDoctorById(doctor_id);
    if (doctor == NULL)
        return;

    for (int day = 0; day < NUM_DAYS_PER_WEEK; day++) {
        for (int shift = 0; shift < NUM_SHIFTS_PER_DAY; shift++) {
            doctor->preference[day][shift] = value;
        }
    }

    printf("All preferences for Dr. %s set to %s.\n", doctor->name,
           value ? "Available" : "Not Available");
}

void clearInputBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF)
        ;
}

int getIntInput() {
    int value;
    char buffer[100];

    if (fgets(buffer, sizeof(buffer), stdin) != NULL) {
        char *endptr;
        value = (int)strtol(buffer, &endptr, 10);

        // Check if the conversion was successful and the whole string was
        // consumed
        if (endptr != buffer && (*endptr == '\n' || *endptr == '\0')) {
            return value;
        }
    }
    // If input is invalid, we can prompt again or handle it.
    // For simplicity, we'll ask the user to retry in the main loop.
    return -1; // Return a value indicating an error
}

char *getStringInput(char *buffer, int size) {
    if (fgets(buffer, size, stdin) != NULL) {
        // Remove newline character if present
        buffer[strcspn(buffer, "\n")] = 0;
        return trimWhitespace(buffer);
    }
    buffer[0] = '\0';
    return buffer;
}
