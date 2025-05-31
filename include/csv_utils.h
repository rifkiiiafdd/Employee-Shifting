#ifndef CSV_UTILS_H
#define CSV_UTILS_H

#include "doctor_list.h" // Include doctor_list.h as CSV functions operate on Doctor structs
#include <stdbool.h> // For boolean type

/**
 * @brief Loads doctor data from a CSV file.
 * The CSV format is assumed to be:
 * Name,MaxShiftsPerWeek,Week1Scheduled,...,Week5Scheduled,SunMorningPref,...,SatNightPref
 * Existing doctors in memory will be cleared before loading.
 *
 * @param filename The path to the CSV file.
 * @return true if data was loaded successfully, false otherwise.
 */
bool loadDoctorsFromCSV(const char *filename);

/**
 * @brief Saves current doctor data from the linked list to a CSV file.
 * The CSV format will include:
 * ID,Name,MaxShiftsPerWeek,Week1Scheduled,...,Week5Scheduled,SunMorningPref,...,SatNightPref
 *
 * @param filename The path to the CSV file.
 * @return true if data was saved successfully, false otherwise.
 */
bool saveDoctorsToCSV(const char *filename);

#endif // CSV_UTILS_H
