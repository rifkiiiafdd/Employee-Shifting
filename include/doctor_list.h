#ifndef DOCTOR_LIST_H
#define DOCTOR_LIST_H

// Library
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Konstanta
#define MAX_NAME_LENGTH 50
#define NUM_WEEKS 5
#define NUM_DAYS_PER_WEEK 7  // 0-6 (Sun-Sat)
#define NUM_SHIFTS_PER_DAY 3 // 0: Morning, 1: Afternoon, 2: Night

// Struktur data penyimpanan dokter
typedef struct Doctor {
        int id;
        char name[MAX_NAME_LENGTH];
        int max_shifts_per_week;
        int shifts_scheduled_per_week[NUM_WEEKS];
        int preference[NUM_DAYS_PER_WEEK][NUM_SHIFTS_PER_DAY];
        struct Doctor *next;
} Doctor;

// Deklarasi variable global
extern Doctor *head;
extern int schedule[NUM_WEEKS][NUM_DAYS_PER_WEEK][NUM_SHIFTS_PER_DAY];

// Deklarasi fungsi (prototype)
void initialize_schedule();
void addDoctor(const char *name, int max_shifts_per_week);
void removeDoctor(int id);
void displayDoctors();
void setDoctorPreference(int doctor_id, int day, int shift, int can_do);
void freeDoctorList();
void refreshDoctorID();
Doctor *findDoctorById(int id);
Doctor *findDoctorByName(const char *name);
void updateDoctor(int id, const char *new_name, int new_max_shifts);

#endif // DOCTOR_LIST_H
