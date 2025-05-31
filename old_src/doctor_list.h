#ifndef DOCTOR_LIST_H
#define DOCTOR_LIST_H

#include <stdbool.h> // For boolean type

// Define constants for array sizes
#define MAX_NAME_LENGTH 50
#define NUM_WEEKS 5
#define NUM_DAYS_PER_WEEK 7
#define NUM_SHIFTS_PER_DAY 3 // 0: Morning, 1: Afternoon, 2: Night

// Enum for shift types for better readability
typedef enum { MORNING = 0, AFTERNOON = 1, NIGHT = 2 } ShiftType;

// Enum for days of the week
typedef enum {
    SUNDAY = 0, // Assuming Sunday is the start of the week for array indexing
    MONDAY = 1,
    TUESDAY = 2,
    WEDNESDAY = 3,
    THURSDAY = 4,
    FRIDAY = 5,
    SATURDAY = 6
} DayOfWeek;

// Structure for a Doctor
typedef struct Doctor {
        int id;                     // Unique identifier for the doctor
        char name[MAX_NAME_LENGTH]; // Doctor's name
        int max_shifts_per_week;    // Maximum shifts allowed per week
        int shifts_scheduled_per_week[NUM_WEEKS]; // Shifts scheduled for each
                                                  // of the 5 weeks
        // Preference matrix: [DayOfWeek][ShiftType]
        // 0: Cannot do this shift (blacklisted), 1: Can do this shift
        int preference[NUM_DAYS_PER_WEEK][NUM_SHIFTS_PER_DAY];
        struct Doctor *next; // Pointer to the next doctor in the linked list
} Doctor;

// Declare the head of the linked list as extern so other files can access it
extern Doctor *head;

// Function prototypes
/**
 * @brief Creates a new Doctor node.
 *
 * @param name The name of the doctor.
 * @param max_shifts The maximum shifts per week.
 * @return A pointer to the newly created Doctor node, or NULL if memory
 * allocation fails.
 */
Doctor *createDoctor(const char *name, int max_shifts);

/**
 * @brief Adds a new doctor to the linked list.
 *
 * @param name The name of the doctor.
 * @param max_shifts The maximum shifts per week.
 */
void addDoctor(const char *name, int max_shifts);

/**
 * @brief Refreshes the IDs of all doctors in the linked list to be sequential
 * starting from 1. This function also updates the global next_doctor_id to
 * reflect the new highest ID + 1.
 */
void refreshDoctorIDs();

/**
 * @brief Removes a doctor from the linked list by ID.
 * After removal, it calls refreshDoctorIDs to re-sequence the IDs.
 *
 * @param id The ID of the doctor to remove.
 */
void removeDoctor(int id);

/**
 * @brief Displays all doctors in the linked list.
 */
void displayDoctors();

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
                         int can_do);

/**
 * @brief Frees all memory allocated for the linked list.
 */
void freeDoctorList();

#endif // DOCTOR_LIST_H
