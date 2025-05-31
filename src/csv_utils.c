#include "csv_utils.h"   // Include its own header
#include "doctor_list.h" // Needed for Doctor struct and linked list functions
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Extern declaration for the head of the doctor list, defined in doctor_list.c
extern Doctor *head;
// Extern declaration for refreshDoctorIDs, defined in doctor_list.c
extern void refreshDoctorIDs();
// Extern declaration for freeDoctorList, defined in doctor_list.c
extern void freeDoctorList();

/**
 * @brief Loads doctor data from a CSV file.
 * The CSV format is assumed to be:
 * ID,Name,MaxShiftsPerWeek,Week1Scheduled,...,Week5Scheduled,SunMorningPref,...,SatNightPref
 * Existing doctors in memory will be cleared before loading.
 *
 * @param filename The path to the CSV file.
 * @return true if data was loaded successfully, false otherwise.
 */
bool loadDoctorsFromCSV(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Failed to open file for reading");
        return false;
    }

    // Clear existing list before loading new data
    freeDoctorList();
    // head = NULL; // freeDoctorList already sets head to NULL

    char line[512]; // Buffer to hold each line from the CSV

    // --- FIX: Skip the header line ---
    if (fgets(line, sizeof(line), file) == NULL) {
        // If file is empty or only has a header, return true for successful
        // (empty) load
        printf("CSV file is empty or only contains a header: %s\n", filename);
        fclose(file);
        return true;
    }

    int loaded_count = 0;
    while (fgets(line, sizeof(line), file) != NULL) {
        // Remove newline character if present
        line[strcspn(line, "\n")] = '\0';

        // Use strtok to parse the CSV line
        char *token;
        char temp_name[MAX_NAME_LENGTH];
        int temp_max_shifts;
        int temp_shifts_scheduled[NUM_WEEKS];
        int temp_preference[NUM_DAYS_PER_WEEK][NUM_SHIFTS_PER_DAY];

        // --- FIX: Read and discard the ID column ---
        token = strtok(line,
                       ","); // This reads the ID, but we don't use it directly
        if (token == NULL)
            continue; // Skip empty lines or malformed lines

        // Parse Name (now the second token after ID)
        token = strtok(NULL, ",");
        if (token == NULL)
            goto parse_error;
        // Remove surrounding quotes if present (for names with commas)
        if (token[0] == '"' && token[strlen(token) - 1] == '"') {
            strncpy(temp_name, token + 1, strlen(token) - 2);
            temp_name[strlen(token) - 2] = '\0';
        } else {
            strncpy(temp_name, token, MAX_NAME_LENGTH - 1);
            temp_name[MAX_NAME_LENGTH - 1] = '\0'; // Ensure null-termination
        }

        // Parse Max Shifts Per Week (now the third token)
        token = strtok(NULL, ",");
        if (token == NULL)
            goto parse_error;
        temp_max_shifts = atoi(token);

        // Parse Shifts Scheduled Per Week (NUM_WEEKS values)
        for (int i = 0; i < NUM_WEEKS; i++) {
            token = strtok(NULL, ",");
            if (token == NULL)
                goto parse_error; // Jump to error handling
            temp_shifts_scheduled[i] = atoi(token);
        }

        // Parse Preferences (NUM_DAYS_PER_WEEK * NUM_SHIFTS_PER_DAY values)
        for (int i = 0; i < NUM_DAYS_PER_WEEK; i++) {
            for (int j = 0; j < NUM_SHIFTS_PER_DAY; j++) {
                token = strtok(NULL, ",");
                if (token == NULL)
                    goto parse_error; // Jump to error handling
                temp_preference[i][j] = atoi(token);
            }
        }

        // Create a new Doctor node
        Doctor *newDoctor = (Doctor *)malloc(sizeof(Doctor));
        if (newDoctor == NULL) {
            printf("Memory allocation failed during CSV load.\n");
            fclose(file);
            return false;
        }

        // Populate the new Doctor node
        // ID is not read from CSV; it will be assigned by refreshDoctorIDs
        newDoctor->id =
            0; // Temporary ID, will be overwritten by refreshDoctorIDs
        strncpy(newDoctor->name, temp_name, MAX_NAME_LENGTH - 1);
        newDoctor->name[MAX_NAME_LENGTH - 1] = '\0';
        newDoctor->max_shifts_per_week = temp_max_shifts;
        for (int i = 0; i < NUM_WEEKS; i++) {
            newDoctor->shifts_scheduled_per_week[i] = temp_shifts_scheduled[i];
        }
        for (int i = 0; i < NUM_DAYS_PER_WEEK; i++) {
            for (int j = 0; j < NUM_SHIFTS_PER_DAY; j++) {
                newDoctor->preference[i][j] = temp_preference[i][j];
            }
        }
        newDoctor->next = NULL;

        // Add to the end of the list (without using addDoctor to control ID
        // assignment)
        if (head == NULL) {
            head = newDoctor;
        } else {
            Doctor *current = head;
            while (current->next != NULL) {
                current = current->next;
            }
            current->next = newDoctor;
        }
        loaded_count++;
    }

    // Re-assign IDs after loading all doctors
    refreshDoctorIDs();
    printf("Successfully loaded %d doctors from %s.\n", loaded_count, filename);
    fclose(file);
    return true;

parse_error:
    printf("Error parsing CSV line (possible incomplete data).\n");
    fclose(file);
    freeDoctorList(); // Clean up partially loaded data
    return false;
}

/**
 * @brief Saves current doctor data from the linked list to a CSV file.
 * The CSV format will include:
 * ID,Name,MaxShiftsPerWeek,Week1Scheduled,...,Week5Scheduled,SunMorningPref,...,SatNightPref
 *
 * @param filename The path to the CSV file.
 * @return true if data was saved successfully, false otherwise.
 */
bool saveDoctorsToCSV(const char *filename) {
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        perror("Failed to open file for writing");
        return false;
    }

    // Write CSV header
    fprintf(file, "ID,Name,MaxShiftsPerWeek");
    for (int i = 1; i <= NUM_WEEKS; i++) {
        fprintf(file, ",Week%dScheduled", i);
    }
    const char *days_short[] = {"Sun", "Mon", "Tue", "Wed",
                                "Thu", "Fri", "Sat"};
    const char *shifts_short[] = {"Morning", "Afternoon", "Night"};
    for (int i = 0; i < NUM_DAYS_PER_WEEK; i++) {
        for (int j = 0; j < NUM_SHIFTS_PER_DAY; j++) {
            fprintf(file, ",%s%sPref", days_short[i], shifts_short[j]);
        }
    }
    fprintf(file, "\n");

    // Write doctor data
    Doctor *current = head;
    while (current != NULL) {
        // Enclose name in quotes to handle potential commas in names
        fprintf(file, "%d,\"%s\",%d", current->id, current->name,
                current->max_shifts_per_week);

        for (int i = 0; i < NUM_WEEKS; i++) {
            fprintf(file, ",%d", current->shifts_scheduled_per_week[i]);
        }

        for (int i = 0; i < NUM_DAYS_PER_WEEK; i++) {
            for (int j = 0; j < NUM_SHIFTS_PER_DAY; j++) {
                fprintf(file, ",%d", current->preference[i][j]);
            }
        }
        fprintf(file, "\n");
        current = current->next;
    }

    printf("Successfully saved doctor data to %s.\n", filename);
    fclose(file);
    return true;
}
