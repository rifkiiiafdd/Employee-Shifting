/* Program: Hitung Total Shift Dokter
    Deskripsi:
    Program ini membaca jadwal shift dokter,j adwal disimpan dalam array dua dimensi schedule[30][3], yang mewakili 30 hari
    dan 3 shift per hari. Setiap elemen array menyimpan ID dokter yang bertugas.
    Input: ID dokter yang ingin dihitung
    Output: Menampilkan total shift yang dijalani dokter dengan ID tersebut
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DAYS 30
#define SHIFTS 3
#define MAX_DOKTER 100

int schedule[DAYS][SHIFTS];

typedef struct {
    int id;
    int totalShift;
    int assigned; // shift yang sudah ditugaskan
} Dokter;

Dokter dokters[MAX_DOKTER];
int dokterCount = 0;

// membaca dari file CSV
void readCSV(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("Gagal membuka file");
        exit(1);
    }

    char line[1024];
    fgets(line, sizeof(line), file);

    while (fgets(line, sizeof(line), file)) {
        int id, w1, w2, w3, w4, w5;
        char* token = strtok(line, ",");
        if (!token) continue;
        id = atoi(token);

        // skip nama
        token = strtok(NULL, ",");

        // skip MaxShiftsPerWeek
        token = strtok(NULL, ",");

        // Week1Scheduled - Week5Scheduled
        token = strtok(NULL, ","); w1 = atoi(token);
        token = strtok(NULL, ","); w2 = atoi(token);
        token = strtok(NULL, ","); w3 = atoi(token);
        token = strtok(NULL, ","); w4 = atoi(token);
        token = strtok(NULL, ","); w5 = atoi(token);

        dokters[dokterCount].id = id;
        dokters[dokterCount].totalShift = w1 + w2 + w3 + w4 + w5;
        dokters[dokterCount].assigned = 0;
        dokterCount++;
    }

    fclose(file);
}

// fungsi menjadwalkan ke schedule[30][3] berdasarkan total shift dokter
void generateSchedule() {
    int current = 0;
    for (int i = 0; i < DAYS; i++) {
        for (int j = 0; j < SHIFTS; j++) {
            int assigned = 0;
            int looped = 0;
            while (looped < dokterCount) {
                if (dokters[current].assigned < dokters[current].totalShift) {
                    schedule[i][j] = dokters[current].id;
                    dokters[current].assigned++;
                    assigned = 1;
                    break;
                }
                current = (current + 1) % dokterCount;
                looped++;
            }

            if (!assigned) {
                schedule[i][j] = 0; // dipakai kalau udah penuh
            }

            current = (current + 1) % dokterCount;
        }
    }
}

// output jadwal
void printSchedule() {
    for (int i = 0; i < DAYS; i++) {
        printf("Hari %2d: ", i + 1);
        for (int j = 0; j < SHIFTS; j++) {
            printf("%2d ", schedule[i][j]);
        }
        printf("\n");
    }
}

// fungsi menghitung total shift seorang dokter
int totalShift(int id) {
    int count = 0;
    for (int i = 0; i < DAYS; i++) {
        for (int j = 0; j < SHIFTS; j++) {
            if (schedule[i][j] == id) {
                count++;
            }
        }
    }
    return count;
}

int main() {
    readCSV("../data/daftar_dokter.csv");
    generateSchedule();
    printSchedule();

    int id;
    printf("\nMasukkan ID dokter untuk menghitung total shift: ");
    scanf("%d", &id);
    printf("Total shift untuk dokter %d: %d shift.\n", id, totalShift(id));

    return 0;
}
