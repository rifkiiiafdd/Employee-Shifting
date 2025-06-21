#include "scheduler.h"
#include "doctor_list.h"
#include <time.h> // Diperlukan untuk srand

Conflict *chead = NULL;

// --- Helper Functions ---
// Fungsi ini bersifat 'static' karena hanya digunakan di dalam file ini
static int check_preference(int id, int day, int shift) {
    Doctor *doctor = findDoctorById(id);
    // FIX: Ditambahkan NULL check untuk keamanan
    if (!doctor) {
        return 0; // Jika dokter tidak ada, dianggap tidak bisa
    }
    // day % NUM_DAYS_PER_WEEK untuk mendapatkan hari dalam seminggu (0-6)
    return doctor->preference[day % NUM_DAYS_PER_WEEK][shift];
}

static void conflict_schedule(int hari, int shift, int id, const char *name) {
    Conflict *newnode = (Conflict *)malloc(sizeof(Conflict));
    if (!newnode)
        return; // Gagal alokasi memori

    newnode->hari = hari;
    newnode->shift = shift;
    newnode->id = id;
    strncpy(newnode->name, name, MAX_NAME_LENGTH - 1);
    newnode->name[MAX_NAME_LENGTH - 1] = '\0';

    newnode->next = chead;
    chead = newnode;
}

// Fungsi untuk membersihkan list conflict untuk mencegah memory leak
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

// Rifki (dengan perbaikan)
void generate_schedule() {
    // 1. Persiapan
    initialize_schedule();
    free_conflict_list(); // Bersihkan konflik dari generasi sebelumnya

    int doctor_count = 0;
    for (Doctor *d = head; d != NULL; d = d->next) {
        doctor_count++;
        // Reset shift mingguan
        for (int i = 0; i < NUM_WEEKS; i++)
            d->shifts_scheduled_per_week[i] = 0;
    }

    if (doctor_count == 0) {
        printf("Jadwal tidak bisa dibuat: Tidak ada dokter dalam daftar.\n");
        return;
    }

    // 2. Iterasi dan Penempatan Awal
    for (int i = 0; i < NUM_DAYS; i++) {
        for (int j = 0; j < NUM_SHIFTS_PER_DAY; j++) {
            int preference_focus = 1;
            int ctr = 0;

            while (ctr <
                   doctor_count *
                       2) { // Loop dengan batasan untuk mencegah infinite loop
                int random_id =
                    (rand() % doctor_count) + 1; // Hasilkan ID acak yang valid
                Doctor *temp = findDoctorById(random_id);
                if (!temp)
                    continue; // Jika ID tidak valid, coba lagi

                int week_of_day = i / NUM_DAYS_PER_WEEK;
                if (temp->shifts_scheduled_per_week[week_of_day] <
                    temp->max_shifts_per_week) {
                    if ((check_preference(temp->id, i, j) &&
                         preference_focus) ||
                        !preference_focus) {
                        schedule[i][j] = temp->id;
                        temp->shifts_scheduled_per_week[week_of_day]++;
                        break;
                    }
                }

                ctr++;
                if (ctr >= doctor_count && preference_focus == 1) {
                    // Jika semua dokter sudah dicek dan tidak ada yang cocok
                    // dengan preferensi, abaikan preferensi.
                    preference_focus = 0;
                }
            }
            if (schedule[i][j] == 0) {
                // Jika masih kosong, assign dokter manapun yang masih bisa
                for (int k = 1; k <= doctor_count; k++) {
                    Doctor *fallback_doc = findDoctorById(k);
                    if (fallback_doc &&
                        fallback_doc
                                ->shifts_scheduled_per_week[i /
                                                            NUM_DAYS_PER_WEEK] <
                            fallback_doc->max_shifts_per_week) {
                        schedule[i][j] = fallback_doc->id;
                        fallback_doc
                            ->shifts_scheduled_per_week[i /
                                                        NUM_DAYS_PER_WEEK]++;
                        break;
                    }
                }
            }
        }
    }

    // 3. Pengganti jadwal (Swapping)
    int id_awal, id_pengganti, alternatif_found;
    for (int i = 0; i < NUM_DAYS; i++) {
        for (int j = 0; j < NUM_SHIFTS_PER_DAY; j++) {
            if (schedule[i][j] != 0 &&
                check_preference(schedule[i][j], i, j) == 0) {
                id_awal = schedule[i][j];
                alternatif_found = 0;

                int start_of_week = i - (i % NUM_DAYS_PER_WEEK);
                // FIX: Logika perulangan yang salah diperbaiki
                for (int k = start_of_week;
                     k < start_of_week + NUM_DAYS_PER_WEEK; k++) {
                    for (int l = 0; l < 3; l++) {
                        if (i == k && j == l)
                            continue; // Jangan menukar dengan diri sendiri

                        id_pengganti = schedule[k][l];
                        if (id_pengganti == 0)
                            continue;

                        if (check_preference(id_awal, k, l) &&
                            check_preference(id_pengganti, i, j)) {
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

    // 4. Mencatat konflik yang tersisa
    for (int i = 0; i < NUM_DAYS; i++) {
        for (int j = 0; j < 3; j++) {
            if (schedule[i][j] != 0 &&
                check_preference(schedule[i][j], i, j) == 0) {
                Doctor *doc = findDoctorById(schedule[i][j]);
                if (doc) { // FIX: Pastikan dokter ada sebelum mengakses nama
                    conflict_schedule(i + 1, j + 1, doc->id, doc->name);
                }
            }
        }
    }
}

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

// --- Implementasi Fungsi Pengambil Jadwal ---

// --- Untuk Dokter Spesifik (Akhyar) ---
int getDoctorScheduleDay(int week, int day, int doctorId, int outShifts[]) {
    if (week < 1 || week > NUM_WEEKS || day < 0 || day >= NUM_DAYS_PER_WEEK)
        return 0;

    int day_index = (week - 1) * NUM_DAYS_PER_WEEK + day;
    int count = 0;
    for (int shift = 0; shift < NUM_SHIFTS_PER_DAY; shift++) {
        if (schedule[day_index][shift] == doctorId) {
            outShifts[count++] = shift;
        }
    }
    return count;
}

void getDoctorScheduleWeek(int week, int doctorId,
                           int outShifts[NUM_DAYS_PER_WEEK][NUM_SHIFTS_PER_DAY],
                           int outCount[NUM_DAYS_PER_WEEK]) {
    if (week < 1 || week > NUM_WEEKS)
        return;
    for (int day = 0; day < NUM_DAYS_PER_WEEK; day++) {
        outCount[day] =
            getDoctorScheduleDay(week, day, doctorId, outShifts[day]);
    }
}

void get_doctor_schedule_month(
    int doctor_id,
    int out_shifts[NUM_WEEKS][NUM_DAYS_PER_WEEK][NUM_SHIFTS_PER_DAY],
    int out_count[NUM_WEEKS][NUM_DAYS_PER_WEEK]) {
    for (int w = 0; w < NUM_WEEKS; w++) {
        getDoctorScheduleWeek(w + 1, doctor_id, out_shifts[w], out_count[w]);
    }
}

// --- Untuk Semua Dokter (Bang Vyto - Modified) ---
void get_all_doctors_schedule_day(int week, int day,
                                  int out_doctor_ids[NUM_SHIFTS_PER_DAY]) {
    if (week < 1 || week > NUM_WEEKS || day < 0 || day >= NUM_DAYS_PER_WEEK) {
        for (int s = 0; s < NUM_SHIFTS_PER_DAY; s++) {
            out_doctor_ids[s] = 0; // Return empty schedule
        }
        return;
    }

    int day_index = (week - 1) * NUM_DAYS_PER_WEEK + day;
    for (int s = 0; s < NUM_SHIFTS_PER_DAY; s++) {
        out_doctor_ids[s] = schedule[day_index][s];
    }
}

void get_all_doctors_schedule_week(
    int week, int out_doctor_ids[NUM_DAYS_PER_WEEK][NUM_SHIFTS_PER_DAY]) {
    if (week < 1 || week > NUM_WEEKS) {
        for (int d = 0; d < NUM_DAYS_PER_WEEK; d++) {
            for (int s = 0; s < NUM_SHIFTS_PER_DAY; s++) {
                out_doctor_ids[d][s] = 0;
            }
        }
        return;
    }
    int start_day_index = (week - 1) * NUM_DAYS_PER_WEEK;
    for (int d = 0; d < NUM_DAYS_PER_WEEK; d++) {
        for (int s = 0; s < NUM_SHIFTS_PER_DAY; s++) {
            out_doctor_ids[d][s] = schedule[start_day_index + d][s];
        }
    }
}

void get_all_doctors_schedule_month(
    int out_doctor_ids[NUM_DAYS][NUM_SHIFTS_PER_DAY]) {
    for (int d = 0; d < NUM_DAYS; d++) {
        for (int s = 0; s < NUM_SHIFTS_PER_DAY; s++) {
            out_doctor_ids[d][s] = schedule[d][s];
        }
    }
}

// --- Fungsi Tampilan Jadwal (Model Lama - untuk referensi) ---
// Bang Vyto
void getScheduleDay(int mingguKe, int hariKe) {
    int index = (mingguKe - 1) * 7 + (hariKe - 1);
    if (index < 0 || index >= NUM_DAYS) { // Check against total days
        printf("Hari tidak valid.\n");
        return;
    }

    printf("{ ");
    for (int i = 0; i < NUM_SHIFTS_PER_DAY; i++) {
        struct Doctor *d = findDoctorById(schedule[index][i]);
        // Display "Kosong" if doctor not found or ID is 0
        printf("\"%s\"", (d ? d->name : "Kosong"));
        if (i < NUM_SHIFTS_PER_DAY - 1)
            printf(", ");
    }
    printf(" }\n");
}

void getScheduleWeek(int mingguKe) {
    int start = (mingguKe - 1) * 7;
    int end = start + 7;

    if (start < 0 || end > NUM_DAYS) {
        printf("Minggu tidak valid.\n");
        return;
    }

    printf("{\n");
    for (int h = start; h < end && h < NUM_DAYS; h++) {
        printf("  { ");
        for (int s = 0; s < NUM_SHIFTS_PER_DAY; s++) {
            struct Doctor *d = findDoctorById(schedule[h][s]);
            printf("\"%s\"", (d ? d->name : "Kosong"));
            if (s < NUM_SHIFTS_PER_DAY - 1)
                printf(", ");
        }
        printf(" }");
        if (h < end - 1)
            printf(",");
        printf("\n");
    }
    printf("}\n");
}

// Dihar

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
    int count = 0;
    for (int i = week * NUM_DAYS_PER_WEEK;
         i < week * NUM_DAYS_PER_WEEK + NUM_DAYS_PER_WEEK; i++) {
        for (int j = 0; j < NUM_SHIFTS_PER_DAY; j++) {
            if (schedule[i][j] == id) {
                count++;
            }
        }
    }
    return count;
}

int totalShiftDay(int week, int day, int id) {
    int count = 0;
    for (int j = 0; j < NUM_SHIFTS_PER_DAY; j++) {
        if (schedule[day][j] == id) {
            count++;
        }
    }
    return count;
}
