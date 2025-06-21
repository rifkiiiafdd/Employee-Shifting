/*
 * File: doctor_list.c
 * Deskripsi: Implementasi fungsi-fungsi untuk mengelola data dokter
 * (menambah, menghapus, mencari, dll.) dalam struktur
 * data linked list
 */

#include "doctor_list.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ----- DEFINISI VARIABEL GLOBAL -----
Doctor *head = NULL; // Pointer yang selalu menunjuk ke dokter pertama di list.
                     // Awalnya NULL karena list kosong.
int next_doctor_id = 1; // ID yang akan diberikan ke dokter baru. Akan bertambah
                        // setiap ada dokter baru.
int schedule[NUM_DAYS][NUM_SHIFTS_PER_DAY]; // Array utama penyimpan jadwal.
                                            // Isinya adalah ID dokter.

// Inisialisasi array jadwal, semua slot diisi 0 (artinya kosong/belum ada
// dokter yang ditugaskan).
void initialize_schedule() {
    for (int d = 0; d < NUM_DAYS; d++) {
        for (int s = 0; s < NUM_SHIFTS_PER_DAY; s++) {
            schedule[d][s] = 0;
        }
    }
}

// Membuat node Doctor baru, tapi belum dimasukkan ke list.
Doctor *createDoctor(const char *name, int max_shifts_per_week) {
    // Alokasikan memori seukuran struct Doctor
    Doctor *newDoctor = (Doctor *)malloc(sizeof(Doctor));
    if (newDoctor == NULL) {
        perror("Alokasi memori untuk dokter baru gagal");
        return NULL;
    }

    // Isi data dokter baru
    newDoctor->id =
        next_doctor_id++; // Pakai ID yang tersedia, lalu naikkan counter ID
    strncpy(newDoctor->name, name, MAX_NAME_LENGTH - 1);
    newDoctor->name[MAX_NAME_LENGTH - 1] =
        '\0'; // Pastikan string diakhiri dengan NULL terminator
    newDoctor->max_shifts_per_week = max_shifts_per_week;

    for (int i = 0; i < NUM_WEEKS; i++) {
        newDoctor->shifts_scheduled_per_week[i] = 0;
    }

    // Atur preferensi default: dokter diasumsikan bisa bekerja kapan saja
    // (nilai 1) Akan diubah dengan setDoctorPreference() jika dokter punya
    // hari/shift yg dia tidak bisa.
    for (int i = 0; i < NUM_DAYS_PER_WEEK; i++) {
        for (int j = 0; j < NUM_SHIFTS_PER_DAY; j++) {
            newDoctor->preference[i][j] = 1; // 1 = Bisa
        }
    }

    newDoctor->next =
        NULL; // Awalnya, dokter baru belum menunjuk ke dokter lain.
    return newDoctor;
}

// Menambahkan dokter baru ke akhir dari linked list.
void addDoctor(const char *name, int max_shifts_per_week) {
    Doctor *newDoctor = createDoctor(name, max_shifts_per_week);
    if (newDoctor == NULL) {
        // Gagal membuat dokter, pesan error sudah ditampilkan di createDoctor
        return;
    }

    // Cek apakah list masih kosong
    if (head == NULL) {
        // Jika ya, dokter baru ini menjadi kepala list
        head = newDoctor;
    } else {
        // Jika tidak, cari elemen terakhir di list
        Doctor *current = head;
        while (current->next != NULL) {
            current = current->next;
        }
        // Sambungkan elemen terakhir ke dokter baru
        current->next = newDoctor;
    }
    printf("Dokter '%s' (ID: %d) berhasil ditambahkan.\n", newDoctor->name,
           newDoctor->id);
}

// Mengurutkan ulang ID semua dokter dan memperbarui jadwal sesuai ID baru.
// Dipanggil setelah ada dokter yang dihapus.
void refreshDoctorIDandSchedule() {
    if (!head) { // Kalau kosong, reset lansung untuk next entry
        next_doctor_id = 1;
        return;
    }

    // Hitung ada berapa dokter sekarang
    int count = 0;
    for (Doctor *d = head; d; d = d->next)
        count++;

    // Buat mapping antara ID lama dan ID baru
    int *old_ids = malloc(sizeof(int) * count);
    int *new_ids = malloc(sizeof(int) * count);

    // Iterasi pertama: ganti ID di list dokter dan simpan mappingnya
    int i = 0;
    for (Doctor *d = head; d; d = d->next, i++) {
        old_ids[i] = d->id; // Simpan ID lama
        new_ids[i] = i + 1; // ID baru adalah urutan dia di list (mulai dari 1)
        d->id = i + 1;      // Update ID dokter di struct
    }
    next_doctor_id = count + 1; // Siapkan ID untuk dokter berikutnya

    // Iterasi kedua: perbarui array 'schedule' dengan ID yang baru
    for (int d = 0; d < NUM_DAYS; d++) {
        for (int s = 0; s < NUM_SHIFTS_PER_DAY; s++) {
            if (schedule[d][s] != 0) { // Hanya proses slot yang terisi
                // Cari ID lama di array mapping
                for (int j = 0; j < count; j++) {
                    if (schedule[d][s] == old_ids[j]) {
                        schedule[d][s] = new_ids[j]; // Ganti dengan ID baru
                        break; // Lanjut ke slot jadwal berikutnya
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

    // Cari dokter yang ingin dihapus, sekaligus menyimpan node sebelumnya
    while (current != NULL && current->id != id) {
        prev = current;
        current = current->next;
    }

    if (current == NULL) {
        printf("Dokter dengan ID %d tidak ditemukan.\n", id);
        return;
    }

    // Langkah 1: Hapus ID dokter ini dari seluruh jadwal
    for (int d = 0; d < NUM_DAYS; d++) {
        for (int s = 0; s < NUM_SHIFTS_PER_DAY; s++) {
            if (schedule[d][s] == id) {
                schedule[d][s] = 0; // Kosongkan slot
            }
        }
    }

    // Langkah 2: Hapus node dokter dari linked list
    if (prev == NULL) {
        // Kalau dokter yang dihapus adalah head
        head = current->next;
    } else {
        // Kalau di tengah atau di akhir, sambung node sebelumnya ke node
        // sesudahnya
        prev->next = current->next;
    }

    printf("Dokter '%s' (ID: %d) berhasil dihapus.\n", current->name, id);
    free(current); // Bebaskan memori dari dokter yang dihapus

    // Langkah 3: Urutkan ulang ID semua dokter yang tersisa dan update jadwal
    refreshDoctorIDandSchedule();
}

// Menampilkan semua dokter dan informasinya ke CLI.
void displayDoctors() {
    if (head == NULL) {
        printf("\n*** Tidak ada dokter dalam daftar. ***\n");
        return;
    }

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
        // Tampilkan jumlah shift per minggu
        for (int i = 0; i < NUM_WEEKS; i++) {
            printf("%-5d", current->shifts_scheduled_per_week[i]);
        }

        // Tampilkan preferensi (hanya yang tidak bisa/nilai 0)
        bool pref_pertama = true;
        for (int d = 0; d < NUM_DAYS_PER_WEEK; d++) {
            for (int s = 0; s < NUM_SHIFTS_PER_DAY; s++) {
                if (current->preference[d][s] == 0) { // 0 = tidak bisa
                    if (!pref_pertama) {
                        printf(", ");
                    }
                    printf("%s %s", nama_hari[d], nama_shift[s]);
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

// Mengatur preferensi seorang dokter untuk shift tertentu.
void setDoctorPreference(int doctor_id, int day, int shift, int can_do) {
    // Validasi input hari dan shift
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

    // Set preferensinya
    doctor->preference[day][shift] = can_do;
    printf("Preferensi untuk Dokter %s telah diperbarui.\n", doctor->name);
}

// Membebaskan semua memori yang dipakai oleh linked list dokter.
void freeDoctorList() {
    Doctor *current = head;
    Doctor *next_node;
    while (current != NULL) {
        next_node = current->next; // Simpan dulu node berikutnya
        free(current);             // Hapus node sekarang
        current = next_node;       // Pindah ke node berikutnya
    }
    head = NULL; // Setelah semua dihapus, head kembali ke NULL
}

// Mencari dokter berdasarkan ID-nya.
Doctor *findDoctorById(int id) {
    // ID harus positif
    if (id <= 0)
        return NULL;

    Doctor *current = head;
    while (current != NULL) {
        if (current->id == id) {
            return current; // Ketemu, langsung return pointernya
        }
        current = current->next;
    }
    return NULL;
}

// Mencari dokter berdasarkan namanya.
Doctor *findDoctorByName(const char *name) {
    Doctor *current = head;
    while (current != NULL) {
        // strcmp utk membandingkan string
        if (strcmp(current->name, name) == 0) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

// Mengupdate informasi dokter yang sudah ada.
void updateDoctor(int id, const char *new_name, int new_max_shifts) {
    Doctor *doctor = findDoctorById(id);
    if (doctor != NULL) {
        // Update nama jika nama baru diberikan (tidak NULL dan tidak kosong)
        if (new_name != NULL && strlen(new_name) > 0) {
            strncpy(doctor->name, new_name, MAX_NAME_LENGTH - 1);
            doctor->name[MAX_NAME_LENGTH - 1] = '\0';
        }
        // Update maks shift jika nilainya valid (lebih dari 0)
        if (new_max_shifts > 0) {
            doctor->max_shifts_per_week = new_max_shifts;
        }
    } else {
        printf("Kesalahan: Tidak dapat menemukan dokter dengan ID %d untuk "
               "diperbarui.\n",
               id);
    }
}
