#include "doctor_list.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Variable Global
Doctor *head = NULL;
int next_doctor_id = 1;
int schedule[NUM_WEEKS][NUM_DAYS_PER_WEEK][NUM_SHIFTS_PER_DAY];

// Implementasi fungsi-fungsi di main (UNTUK CLI SAJA, BUKAN GUI)

// Membuat dan mengosongkan array schedule ke 0 (Tidak terassign dokter
// siapapun)
void initialize_schedule() {
    for (int w = 0; w < NUM_WEEKS; w++) {
        for (int d = 0; d < NUM_DAYS_PER_WEEK; d++) {
            for (int s = 0; s < NUM_SHIFTS_PER_DAY; s++) {
                schedule[w][d][s] = 0; // 0 -> Tidak terassign
            }
        }
    }
}

// Buat struct dokter
Doctor *createDoctor(const char *name, int max_shifts_per_week) {
    Doctor *newDoctor = (Doctor *)malloc(sizeof(Doctor));
    if (newDoctor == NULL) {
        perror("Memory allocation failed for new doctor");
        return NULL;
    }

    newDoctor->id = next_doctor_id++;
    strncpy(newDoctor->name, name, MAX_NAME_LENGTH - 1);
    newDoctor->name[MAX_NAME_LENGTH - 1] = '\0';
    newDoctor->max_shifts_per_week = max_shifts_per_week;

    for (int i = 0; i < NUM_WEEKS; i++) {
        newDoctor->shifts_scheduled_per_week[i] = 0;
    }

    // Inisialisasi preferensi masing-masing dokter, sebelum dimasukkan
    // preferensi jika ada (sistem blacklist)
    for (int i = 0; i < NUM_DAYS_PER_WEEK; i++) {
        for (int j = 0; j < NUM_SHIFTS_PER_DAY; j++) {
            newDoctor->preference[i][j] = 1;
        }
    }

    newDoctor->next = NULL;
    return newDoctor;
}

// Menambahkan dokter ke linked list
void addDoctor(const char *name, int max_shifts_per_week) {
    Doctor *newDoctor = createDoctor(name, max_shifts_per_week);
    if (newDoctor == NULL) {
        return;
    }

    if (head == NULL) {
        head = newDoctor;
    } else {
        Doctor *current = head;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = newDoctor;
    }
    printf("Doctor '%s' (ID: %d) added successfully.\n", newDoctor->name,
           newDoctor->id);
}

// Refresh ID Dokter (Jika diedit, maka jadwal harus diubah juga)
void refreshDoctorID() {
    if (head == NULL) {
        next_doctor_id = 1;
        return;
    }

    int max_id = 0;
    Doctor *current = head;
    while (current != NULL) {
        if (current->id > max_id) {
            max_id = current->id;
        }
        current = current->next;
    }
    next_doctor_id = max_id + 1;
}

// Menghapus dokter dari linked list berdasarkan ID
void removeDoctor(int id) {
    Doctor *current = head;
    Doctor *prev = NULL;

    while (current != NULL && current->id != id) {
        prev = current;
        current = current->next;
    }

    if (current == NULL) {
        printf("Doctor (ID: %d) not found.\n", id);
        return;
    }

    if (prev == NULL) {
        head = current->next; // Removing the head node
    } else {
        prev->next = current->next;
    }

    printf("Doctor '%s' (ID: %d) removed successfully.\n", current->name, id);
    free(current);
    refreshDoctorID();
}

// Menampilkan semua dokter dan informasinya
void displayDoctors() {
    if (head == NULL) {
        printf("\n*** No doctors in the list. ***\n");
        return;
    }

    const char *day_names[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
    const char *shift_names[] = {"Morning", "Afternoon", "Night"};

    printf("\n--- Doctor List ---\n");
    printf("%-5s %-25s %-15s %s\n", "ID", "Name", "Max Shifts/Wk",
           "Unavailable Shifts");
    printf("-------------------------------------------------------------------"
           "-------------\n");

    Doctor *current = head;
    while (current != NULL) {
        // Tampilkan data dokter biasa
        printf("%-5d %-25s %-15d ", current->id, current->name,
               current->max_shifts_per_week);

        // Tampilkan preferensi (Hanya yang tidak bisa)
        bool first_pref = true;
        for (int d = 0; d < NUM_DAYS_PER_WEEK; d++) {
            for (int s = 0; s < NUM_SHIFTS_PER_DAY; s++) {
                if (current->preference[d][s] == 0) {
                    if (!first_pref) {
                        printf(", ");
                    }
                    printf("%s %s", day_names[d], shift_names[s]);
                    first_pref = false;
                }
            }
        }
        printf("\n");
        current = current->next;
    }
    printf("-------------------------------------------------------------------"
           "-------------\n");
}

// Buat preferensi seorang dokter untuk shift tertentu
void setDoctorPreference(int doctor_id, int day, int shift, int can_do) {
    if (day < 0 || day >= NUM_DAYS_PER_WEEK || shift < 0 ||
        shift >= NUM_SHIFTS_PER_DAY) {
        printf("Invalid day or shift type provided.\n");
        return;
    }

    Doctor *doctor = findDoctorById(doctor_id);
    if (doctor == NULL) {
        printf("Doctor with ID %d not found.\n", doctor_id);
        return;
    }

    doctor->preference[day][shift] = can_do;
    printf("Preference for Doctor %s updated.\n", doctor->name);
}

// Free semua memori list dokter
void freeDoctorList() {
    Doctor *current = head;
    Doctor *next_node;
    while (current != NULL) {
        next_node = current->next;
        free(current);
        current = next_node;
    }
    head = NULL;
}

// Cari dokter berdasarkan ID
Doctor *findDoctorById(int id) {
    Doctor *current = head;
    while (current != NULL) {
        if (current->id == id) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

// Cari dokter berdasarkan nama
Doctor *findDoctorByName(const char *name) {
    Doctor *current = head;
    while (current != NULL) {
        if (strcmp(current->name, name) == 0) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

// Mengupdate informasi dokter jika diedit
void updateDoctor(int id, const char *new_name, int new_max_shifts) {
    Doctor *doctor = findDoctorById(id);
    if (doctor != NULL) {
        // Only update name if a new one is provided
        if (strlen(new_name) > 0) {
            strncpy(doctor->name, new_name, MAX_NAME_LENGTH - 1);
            doctor->name[MAX_NAME_LENGTH - 1] = '\0';
        }
        // Only update shifts if a positive value is given
        if (new_max_shifts > 0) {
            doctor->max_shifts_per_week = new_max_shifts;
        }
    } else {
        printf("Error: Could not find doctor with ID %d to update.\n", id);
    }
}
