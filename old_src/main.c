#include "csv_utils.h"   // Include the new header for CSV utility functions
#include "doctor_list.h" // Include the header file for doctor list functions
#include <stdio.h>

int main() {
    const char *data_file = "daftar_dokter.csv";
    const char *schedule_output_file =
        "jadwal.csv"; // Placeholder for future schedule output

    // --- Scenario 1: Add doctors and save to CSV ---
    printf("--- Scenario 1: Adding doctors and saving to CSV ---\n");
    addDoctor("Dr. Alice", 20);
    addDoctor("Dr. Bob", 18);
    addDoctor("Dr. Charlie", 22);
    displayDoctors();
    saveDoctorsToCSV(data_file);

    // --- Scenario 2: Clear current list and load from CSV ---
    printf("\n--- Scenario 2: Clearing list and loading from CSV ---\n");
    // Free the current list to simulate starting fresh or loading from file
    freeDoctorList();
    displayDoctors(); // Should show no doctors

    // Load doctors from the CSV file we just saved
    if (loadDoctorsFromCSV(data_file)) {
        printf("\nDoctors loaded from CSV. Current list:\n");
        displayDoctors(); // Should show Alice, Bob, Charlie with refreshed IDs
    } else {
        printf("Failed to load doctors from CSV.\n");
    }

    // --- Scenario 3: Modify loaded data and save again ---
    printf("\n--- Scenario 3: Modifying loaded data and saving again ---\n");
    // Assume Dr. Bob is now ID 2 after refresh, or look up by name/original ID
    // if needed For simplicity, let's assume we know current IDs from
    // displayDoctors
    setDoctorPreference(1, MONDAY, MORNING,
                        0); // Dr. Alice cannot do Monday Morning
    setDoctorPreference(2, FRIDAY, NIGHT, 0); // Dr. Bob cannot do Friday Night
    removeDoctor(3);                          // Remove Dr. Charlie (now ID 3)
    addDoctor("Dr. Eve", 17);                 // Add a new doctor
    displayDoctors();
    saveDoctorsToCSV(data_file); // Save the modified list

    // --- Scenario 4: Load modified data again to verify ---
    printf("\n--- Scenario 4: Loading modified data to verify ---\n");
    freeDoctorList();
    if (loadDoctorsFromCSV(data_file)) {
        printf("\nDoctors loaded from modified CSV. Current list:\n");
        displayDoctors(); // Should show Alice, Bob, Eve with their updated
                          // states
    } else {
        printf("Failed to load doctors from CSV.\n");
    }

    // Free all allocated memory before program exits
    freeDoctorList();

    return 0;
}
