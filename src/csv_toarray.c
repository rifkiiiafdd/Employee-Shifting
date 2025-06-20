#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_DOKTER 100
#define DAYS 30
#define SHIFTS 3

int schedule[DAYS][SHIFTS];

typedef struct {
    int id;
    int totalShift; // Total dari Week1Scheduled sampai Week5Scheduled
    int assigned;   // Untuk tracking berapa shift yang sudah diberikan
} Dokter;

Dokter dokters[MAX_DOKTER];
int dokterCount = 0;

// Fungsi untuk split dan parsing CSV
void readCSV(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("Gagal membuka file %s\n", filename);
        exit(1);
    }

    char line[512];
    fgets(line, sizeof(line), file); // Skip header

    while (fgets(line, sizeof(line), file)) {
        int id, week1, week2, week3, week4, week5;
        char* token = strtok(line, ","); // ID
        if (!token) continue;
        id = atoi(token);

        // Skip name
        token = strtok(NULL, ",");

        // Skip MaxShiftsPerWeek
        token = strtok(NULL, ",");

        // Week1Scheduled sampai Week5Scheduled
        token = strtok(NULL, ",");
        week1 = atoi(token);
        token = strtok(NULL, ",");
        week2 = atoi(token);
        token = strtok(NULL, ",");
        week3 = atoi(token);
        token = strtok(NULL, ",");
        week4 = atoi(token);
        token = strtok(NULL, ",");
        week5 = atoi(token);

        dokters[dokterCount].id = id;
        dokters[dokterCount].totalShift = week1 + week2 + week3 + week4 + week5;
        dokters[dokterCount].assigned = 0;
        dokterCount++;
    }

    fclose(file);
}

// Fungsi untuk membagi shift ke dalam schedule[30][3]
void generateSchedule() {
    int totalSlots = DAYS * SHIFTS;
    int current = 0;

    for (int i = 0; i < DAYS; i++) {
        for (int j = 0; j < SHIFTS; j++) {
            int assigned = 0;
            for (int k = 0; k < dokterCount; k++) {
                if (dokters[current].assigned < dokters[current].totalShift) {
                    schedule[i][j] = dokters[current].id;
                    dokters[current].assigned++;
                    assigned = 1;
                    break;
                }
                current = (current + 1) % dokterCount;
            }

            // Jika tidak ada dokter yang bisa diassign, isi dengan 0
            if (!assigned) {
                schedule[i][j] = 0;
            }

            current = (current + 1) % dokterCount;
        }
    }
}

// Fungsi untuk mencetak schedule
void printSchedule() {
    for (int i = 0; i < DAYS; i++) {
        printf("Hari %2d: ", i + 1);
        for (int j = 0; j < SHIFTS; j++) {
            printf("%d ", schedule[i][j]);
        }
        printf("\n");
    }
}

int main() {
    readCSV("../data/daftar_dokter.csv"); 
    for (int i = 0; i < dokterCount; i++) {
    printf("Dokter ID %d: totalShift = %d\n", dokters[i].id, dokters[i].totalShift);
}
    generateSchedule();
    printSchedule();
    return 0;
}

