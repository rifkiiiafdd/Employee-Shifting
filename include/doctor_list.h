#ifndef DOCTOR_LIST_H
#define DOCTOR_LIST_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

typedef struct Doctor {
        int id;
        char name[MAX_NAME_LENGTH];
        int max_shifts_per_week;
        int shifts_scheduled_per_week[NUM_WEEKS];

        int preference[NUM_DAYS_PER_WEEK][NUM_SHIFTS_PER_DAY];
        struct Doctor *next;
} Doctor;

#endif // !DOCTOR_LIST_H
