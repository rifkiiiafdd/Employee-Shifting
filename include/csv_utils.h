#ifndef CSV_UTILS_H
#define CSV_UTILS_H

#include <stdbool.h>

// Deklarasi fungsi (Prototype)
bool save_doctors_to_csv(const char *filepath);
bool load_doctors_from_csv(const char *filepath);
bool save_schedule_to_csv(const char *filepath);
bool load_schedule_from_csv(const char *filepath);

#endif // CSV_UTILS_H
