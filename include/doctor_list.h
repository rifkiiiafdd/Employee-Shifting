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

typedef enum { MORNING = 0, AFTERNOON = 1, NIGHT = 2 } ShiftType;

typedef enum {
    SUNDAY = 0,
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

extern Doctor *head;

void addDoctor(const char *name, int max_shifts_per_week);
void removeDoctor(int id);
void displayDoctors();
void setDoctorPreference(int doctor_id, DayOfWeek day, ShiftType shift,
                         int can_do);
void freeDoctorList();
Doctor *findDoctorById(int id);
Doctor *findDoctorByName(const char *name);
void updateDoctor(int id, const char *new_name, int new_max_shifts);

#endif // DOCTOR_LIST_H
