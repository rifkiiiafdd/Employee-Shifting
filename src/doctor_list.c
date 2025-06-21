#include "doctor_list.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Variable Global
Doctor *head = NULL;
int next_doctor_id = 1;
int schedule[NUM_DAYS][NUM_SHIFTS_PER_DAY];

// Membuat dan mengosongkan array schedule ke 0 (Tidak terassign dokter
// siapapun)
void initialize_schedule() {
    for (int d = 0; d < NUM_DAYS; d++) {
        for (int s = 0; s < NUM_SHIFTS_PER_DAY; s++) {
            schedule[d][s] = 0;
        }
    }
}

// Buat struct dokter
Doctor *createDoctor(const char *name, int max_shifts_per_week) {
    Doctor *newDoctor = (Doctor *)malloc(sizeof(Doctor));
    if (newDoctor == NULL) {
        perror("Alokasi memori untuk dokter baru gagal");
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
    printf("Dokter '%s' (ID: %d) berhasil ditambahkan.\n", newDoctor->name,
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

void refreshDoctorIDandSchedule() {
    if (!head) {
        next_doctor_id = 1;
        return;
    }

    int count = 0;
    for (Doctor *d = head; d; d = d->next)
        count++;

    int *old_ids = malloc(sizeof(int) * count);
    int *new_ids = malloc(sizeof(int) * count);

    int i = 0;
    for (Doctor *d = head; d; d = d->next, i++) {
        old_ids[i] = d->id;
        new_ids[i] = i + 1;
        d->id = i + 1;
    }
    next_doctor_id = count + 1;

    for (int d = 0; d < NUM_DAYS; d++) {
        for (int s = 0; s < NUM_SHIFTS_PER_DAY; s++) {
            if (schedule[d][s] != 0) {
                for (int j = 0; j < count; j++) {
                    if (schedule[d][s] == old_ids[j]) {
                        schedule[d][s] = new_ids[j];
                        break;
                    }
                }
            }
        }
    }

    free(old_ids);
    free(new_ids);
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
        printf("Doctor with ID %d not found.\n", id);
        return;
    }

    // Step 1: Scrub the schedule of the doctor being removed.
    for (int d = 0; d < NUM_DAYS; d++) {
        for (int s = 0; s < NUM_SHIFTS_PER_DAY; s++) {
            if (schedule[d][s] == id) {
                schedule[d][s] = 0; // Unassign shift
            }
        }
    }

    // Step 2: Remove the doctor from the linked list.
    if (prev == NULL) {
        head = current->next;
    } else {
        prev->next = current->next;
    }

    printf("Doctor '%s' (ID: %d) successfully removed.\n", current->name, id);
    free(current);

    // Step 3: Resequence all remaining doctor IDs and update the schedule.
    refreshDoctorIDandSchedule();
}

// Menampilkan semua dokter dan informasinya
void displayDoctors() {
    if (head == NULL) {
        printf("\n*** Tidak ada dokter dalam daftar. ***\n");
        return;
    }

    // --- PERUBAHAN DI SINI ---
    // Nama hari diubah urutannya agar Senin menjadi yang pertama (indeks 0)
    const char *nama_hari[] = {"Senin", "Selasa", "Rabu",  "Kamis",
                               "Jumat", "Sabtu",  "Minggu"};
    const char *nama_shift[] = {"Pagi", "Siang", "Malam"};

    printf("\n--- Daftar Dokter ---\n");
    printf("%-5s %-25s %-15s %-25s %s\n", "ID", "Nama", "Maks Shift/Mgg",
           "Shift Terjadwal/Minggu", "Shift Tidak Tersedia");
    printf("-------------------------------------------------------------------"
           "----------------------\n");

    Doctor *current = head;
    while (current != NULL) {
        printf("%-5d %-25s %-15d ", current->id, current->name,
               current->max_shifts_per_week);
        printf("%-5d %-5d %-5d %-5d %-5d",
               current->shifts_scheduled_per_week[0],
               current->shifts_scheduled_per_week[1],
               current->shifts_scheduled_per_week[2],
               current->shifts_scheduled_per_week[3],
               current->shifts_scheduled_per_week[4]);

        // Tampilkan preferensi (Hanya yang tidak bisa)
        bool pref_pertama = true;
        for (int d = 0; d < NUM_DAYS_PER_WEEK; d++) {
            for (int s = 0; s < NUM_SHIFTS_PER_DAY; s++) {
                if (current->preference[d][s] == 0) {
                    if (!pref_pertama) {
                        printf(", ");
                    }
                    printf("%s %s", nama_hari[d],
                           nama_shift[s]); // Akan menampilkan nama hari yang
                                           // benar
                    pref_pertama = false;
                }
            }
        }
        if (pref_pertama) {
            printf("Tidak ada");
        }
        printf("\n");
        current = current->next;
    }
    printf("-------------------------------------------------------------------"
           "----------------------\n");
}

// Buat preferensi seorang dokter untuk shift tertentu
void setDoctorPreference(int doctor_id, int day, int shift, int can_do) {
    if (day < 0 || day >= NUM_DAYS_PER_WEEK || shift < 0 ||
        shift >= NUM_SHIFTS_PER_DAY) {
        printf("Hari atau jenis shift yang diberikan tidak valid.\n");
        return;
    }

    Doctor *doctor = findDoctorById(doctor_id);
    if (doctor == NULL) {
        printf("Dokter dengan ID %d tidak ditemukan.\n", doctor_id);
        return;
    }

    doctor->preference[day][shift] = can_do;
    printf("Preferensi untuk Dokter %s telah diperbarui.\n", doctor->name);
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
    if (id <= 0)
        return NULL;
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
        if (new_name != NULL && strlen(new_name) > 0) {
            strncpy(doctor->name, new_name, MAX_NAME_LENGTH - 1);
            doctor->name[MAX_NAME_LENGTH - 1] = '\0';
        }
        if (new_max_shifts > 0) {
            doctor->max_shifts_per_week = new_max_shifts;
        }
    } else {
        printf("Kesalahan: Tidak dapat menemukan dokter dengan ID %d untuk "
               "diperbarui.\n",
               id);
    }
}
