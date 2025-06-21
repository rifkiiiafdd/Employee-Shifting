/*
 * File: scheduler.c
 * Deskripsi: Implementasi algoritma untuk membuat jadwal dokter secara
 * otomatis, mendeteksi konflik, dan menyediakan fungsi-fungsi untuk mengambil
 * data jadwal
 */

#include "scheduler.h"
#include "doctor_list.h"
#include <time.h>

// ----- DEFINISI VARIABEL GLOBAL -----
Conflict *chead =
    NULL; // Kepala (head) dari linked list yang menyimpan data konflik jadwal.

// ----- FUNGSI BANTU (STATIC) -----

// Cek apakah seorang dokter bersedia (punya preferensi 1) pada hari dan shift
// tertentu.
static int check_preference(int id, int day, int shift) {
    Doctor *doctor = findDoctorById(id);
    if (!doctor) {
        return 0; // Jika dokter tidak ditemukan, anggap tidak bersedia.
    }
    // Preferensi disimpan per hari dalam seminggu, jadi gunakan modulo 7
    return doctor->preference[day % NUM_DAYS_PER_WEEK][shift];
}

// Menambahkan data konflik baru ke awal linked list 'chead'.
static void conflict_schedule(int hari, int shift, int id, const char *name) {
    Conflict *newnode = (Conflict *)malloc(sizeof(Conflict));
    if (!newnode)
        return; // Gagal alokasi memori

    newnode->hari = hari;
    newnode->shift = shift;
    newnode->id = id;
    strncpy(newnode->name, name, MAX_NAME_LENGTH - 1);
    newnode->name[MAX_NAME_LENGTH - 1] = '\0';

    // Sambungkan node baru ke depan list
    newnode->next = chead;
    chead = newnode;
}

// ----- IMPLEMENTASI FUNGSI UTAMA -----

// Membebaskan semua memori dari linked list konflik.
void free_conflict_list() {
    Conflict *current = chead;
    Conflict *next_node;
    while (current != NULL) {
        next_node = current->next;
        free(current);
        current = next_node;
    }
    chead = NULL;
}

// Fungsi utama untuk menghasilkan jadwal secara otomatis.
void generate_schedule() {
    // --- Tahap 1: Persiapan ---
    initialize_schedule(); // Kosongkan jadwal lama
    free_conflict_list();  // Kosongkan daftar konflik lama

    // Hitung jumlah dokter yang ada
    int doctor_count = 0;
    for (Doctor *d = head; d != NULL; d = d->next) {
        doctor_count++;
        // Reset juga counter shift per minggu untuk setiap dokter
        for (int i = 0; i < NUM_WEEKS; i++)
            d->shifts_scheduled_per_week[i] = 0;
    }

    if (doctor_count == 0) {
        printf("Jadwal tidak bisa dibuat: Tidak ada dokter dalam daftar.\n");
        return;
    }

    // --- Tahap 2: Penempatan Awal (Iterasi Acak) ---
    // Loop untuk setiap hari dan setiap shift
    for (int i = 0; i < NUM_DAYS; i++) {
        for (int j = 0; j < NUM_SHIFTS_PER_DAY; j++) {
            // Coba cari dokter yang cocok secara acak
            int ctr = 0;
            // Loop ini akan mencoba sebanyak (jumlah_dokter * 2) kali untuk
            // menemukan dokter yang pas. Ini untuk mencegah infinite loop jika
            // tidak ada dokter yang cocok.
            while (ctr < doctor_count * 2) {
                int random_id =
                    (rand() % doctor_count) + 1; // Ambil ID dokter acak
                Doctor *temp = findDoctorById(random_id);
                if (!temp)
                    continue; // Jika ID tidak valid (seharusnya tidak terjadi),
                              // coba lagi

                int week_of_day = i / NUM_DAYS_PER_WEEK;

                // Cek apakah dokter masih punya kuota shift minggu ini
                if (temp->shifts_scheduled_per_week[week_of_day] <
                    temp->max_shifts_per_week) {
                    // Cek preferensi dokter
                    if (check_preference(temp->id, i, j)) {
                        schedule[i][j] = temp->id; // Jadwalkan dokter ini
                        temp->shifts_scheduled_per_week[week_of_day]++;
                        break; // Lanjut ke shift berikutnya
                    }
                }
                ctr++;
            }

            // Jika setelah percobaan acak slot masih kosong (misal karena semua
            // yg terpilih acak tidak cocok preferensinya)
            if (schedule[i][j] == 0) {
                // Lakukan pencarian berurutan sebagai fallback, abaikan
                // preferensi.
                for (int k = 1; k <= doctor_count; k++) {
                    Doctor *fallback_doc = findDoctorById(k);
                    int week_of_day = i / NUM_DAYS_PER_WEEK;
                    if (fallback_doc &&
                        fallback_doc->shifts_scheduled_per_week[week_of_day] <
                            fallback_doc->max_shifts_per_week) {
                        schedule[i][j] = fallback_doc->id;
                        fallback_doc->shifts_scheduled_per_week[week_of_day]++;
                        break; // Dapet dokter, lanjut.
                    }
                }
            }
        }
    }

    // --- Tahap 3: Optimasi Jadwal (Swapping) ---
    // Tahap ini mencoba memperbaiki konflik dengan menukar jadwal antar dokter.
    int id_awal, id_pengganti, alternatif_found;
    for (int i = 0; i < NUM_DAYS; i++) {
        for (int j = 0; j < NUM_SHIFTS_PER_DAY; j++) {
            // Cek apakah ada dokter yang dijadwalkan di luar preferensinya
            if (schedule[i][j] != 0 &&
                check_preference(schedule[i][j], i, j) == 0) {
                id_awal = schedule[i][j]; // Ini dokter yg konflik
                alternatif_found = 0;

                // Cari jadwal lain di minggu yang sama untuk ditukar
                int start_of_week = i - (i % NUM_DAYS_PER_WEEK);
                for (int k = start_of_week;
                     k < start_of_week + NUM_DAYS_PER_WEEK; k++) {
                    for (int l = 0; l < 3; l++) {
                        if (i == k && j == l)
                            continue; // Jangan tukar dengan diri sendiri

                        id_pengganti =
                            schedule[k][l]; // Ini kandidat untuk tukeran
                        if (id_pengganti == 0)
                            continue;

                        // Kondisi ideal: dokter awal cocok di jadwal baru, DAN
                        // dokter pengganti cocok di jadwal lama.
                        if (check_preference(id_awal, k, l) &&
                            check_preference(id_pengganti, i, j)) {
                            // Tukar posisi!
                            schedule[i][j] = id_pengganti;
                            schedule[k][l] = id_awal;
                            alternatif_found = 1;
                            break;
                        }
                    }
                    if (alternatif_found == 1)
                        break;
                }
            }
        }
    }

    // --- Tahap 4: Pencatatan Konflik Final ---
    // Setelah dioptimasi, catat konflik yang masih tersisa.
    for (int i = 0; i < NUM_DAYS; i++) {
        for (int j = 0; j < 3; j++) {
            if (schedule[i][j] != 0 &&
                check_preference(schedule[i][j], i, j) == 0) {
                Doctor *doc = findDoctorById(schedule[i][j]);
                if (doc) {
                    // Masukkan ke daftar konflik
                    conflict_schedule(i, j, doc->id, doc->name);
                }
            }
        }
    }
}

// Menampilkan konflik yang tercatat ke konsol.
void view_conflict() {
    if (chead == NULL) {
        printf("\nTidak ada konflik jadwal yang ditemukan.\n");
        return;
    }
    printf("\n--- Konflik Jadwal yang Tidak Terselesaikan ---\n");
    Conflict *temp = chead;
    while (temp != NULL) {
        printf("Dokter %s (ID: %d) di hari ke-%d, shift ke-%d\n", temp->name,
               temp->id, temp->hari, temp->shift);
        temp = temp->next;
    }
    printf("---------------------------------------------\n");
}

// ----- IMPLEMENTASI FUNGSI GETTER -----

// --- Untuk Dokter Spesifik ---

void getDoctorScheduleDay(int week, int day, int doctorId, int outShifts[],
                          int *outCount) {
    *outCount = 0; // Inisialisasi count
    if (week < 1 || week > NUM_WEEKS || day < 0 || day >= NUM_DAYS_PER_WEEK)
        return; // Input tidak valid

    // Hitung indeks hari absolut dari 0-29
    int day_index = (week - 1) * NUM_DAYS_PER_WEEK + day;
    if (day_index >= NUM_DAYS)
        return;

    int count = 0;
    for (int shift = 0; shift < NUM_SHIFTS_PER_DAY; shift++) {
        if (schedule[day_index][shift] == doctorId) {
            outShifts[count++] = shift; // Simpan jenis shift-nya (0/1/2)
        }
    }
    *outCount = count; // Simpan hasil hitungan ke pointer output
}

void getDoctorScheduleWeek(int week, int doctorId,
                           int outShifts[NUM_DAYS_PER_WEEK][NUM_SHIFTS_PER_DAY],
                           int outCount[NUM_DAYS_PER_WEEK]) {
    if (week < 1 || week > NUM_WEEKS)
        return; // Input tidak valid
    // Panggil getDoctorScheduleDay untuk setiap hari dalam seminggu
    for (int day = 0; day < NUM_DAYS_PER_WEEK; day++) {
        getDoctorScheduleDay(week, day, doctorId, outShifts[day],
                             &outCount[day]);
    }
}

void get_doctor_schedule_month(
    int doctor_id,
    int out_shifts[NUM_WEEKS][NUM_DAYS_PER_WEEK][NUM_SHIFTS_PER_DAY],
    int out_count[NUM_WEEKS][NUM_DAYS_PER_WEEK]) {
    // Panggil getDoctorScheduleWeek untuk setiap minggu
    for (int w = 0; w < NUM_WEEKS; w++) {
        getDoctorScheduleWeek(w + 1, doctor_id, out_shifts[w], out_count[w]);
    }
}

// --- Untuk Semua Dokter ---

void get_all_doctors_schedule_day(int week, int day,
                                  int out_doctor_ids[NUM_SHIFTS_PER_DAY]) {
    if (week < 1 || week > NUM_WEEKS || day < 0 || day >= NUM_DAYS_PER_WEEK) {
        // Jika input tidak valid, isi output dengan 0 (kosong)
        for (int s = 0; s < NUM_SHIFTS_PER_DAY; s++)
            out_doctor_ids[s] = 0;
        return;
    }

    int day_index = (week - 1) * NUM_DAYS_PER_WEEK + day;
    if (day_index >= NUM_DAYS)
        return;

    for (int s = 0; s < NUM_SHIFTS_PER_DAY; s++) {
        out_doctor_ids[s] = schedule[day_index][s];
    }
}

void get_all_doctors_schedule_week(
    int week, int out_doctor_ids[NUM_DAYS_PER_WEEK][NUM_SHIFTS_PER_DAY]) {
    if (week < 1 || week > NUM_WEEKS) {
        // Jika input tidak valid, kosongkan semua
        for (int d = 0; d < NUM_DAYS_PER_WEEK; d++) {
            for (int s = 0; s < NUM_SHIFTS_PER_DAY; s++)
                out_doctor_ids[d][s] = 0;
        }
        return;
    }
    for (int d = 0; d < NUM_DAYS_PER_WEEK; d++) {
        get_all_doctors_schedule_day(week, d, out_doctor_ids[d]);
    }
}

void get_all_doctors_schedule_month(
    int out_doctor_ids[NUM_DAYS][NUM_SHIFTS_PER_DAY]) {
    // Cukup salin seluruh isi array schedule global ke array output
    for (int d = 0; d < NUM_DAYS; d++) {
        for (int s = 0; s < NUM_SHIFTS_PER_DAY; s++) {
            out_doctor_ids[d][s] = schedule[d][s];
        }
    }
}

// ----- FUNGSI UTILITAS PENGHITUNGAN -----

int totalShift(int id) {
    int count = 0;
    for (int i = 0; i < NUM_DAYS; i++) {
        for (int j = 0; j < NUM_SHIFTS_PER_DAY; j++) {
            if (schedule[i][j] == id) {
                count++;
            }
        }
    }
    return count;
}

int totalShiftWeek(int week, int id) {
    if (week < 1 || week > NUM_WEEKS)
        return 0; // Validasi input
    int count = 0;
    int start_day = (week - 1) * NUM_DAYS_PER_WEEK;
    int end_day = start_day + NUM_DAYS_PER_WEEK;

    for (int i = start_day; i < end_day && i < NUM_DAYS; i++) {
        for (int j = 0; j < NUM_SHIFTS_PER_DAY; j++) {
            if (schedule[i][j] == id) {
                count++;
            }
        }
    }
    return count;
}

int totalShiftDay(int week, int day, int id) {
    if (week < 1 || week > NUM_WEEKS || day < 0 || day >= NUM_DAYS_PER_WEEK)
        return 0;

    int day_index = (week - 1) * NUM_DAYS_PER_WEEK + day;
    if (day_index >= NUM_DAYS)
        return 0;

    int count = 0;
    for (int j = 0; j < NUM_SHIFTS_PER_DAY; j++) {
        if (schedule[day_index][j] == id) {
            count++;
        }
    }
    return count;
}
