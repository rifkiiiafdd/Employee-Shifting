/*
spesifikasi : Menampilkan data dokter dengan nama atau indeks tertentu
input  : ID atau nama dokter
output : Informasi lengkap dokter dalam format yang ditentukan
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH 1024
#define MAX_NAME_LENGTH 100
#define MAX_WEEKS 5
#define DAYS_PER_WEEK 7
#define SHIFTS_PER_DAY 3

// Struktur untuk menyimpan data dokter
typedef struct {
    int id;
    char name[MAX_NAME_LENGTH];
    int maxShiftsPerWeek;
    int scheduledShifts[MAX_WEEKS];
    int preferences[DAYS_PER_WEEK][SHIFTS_PER_DAY];
} Doctor;

// Fungsi untuk membaca data dokter dari file CSV
Doctor bacaDataDokter(const char *filename, int targetId) {
    Doctor doc = {0};
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Error: Tidak dapat membuka file %s\n", filename);
        return doc;
    }

    char line[MAX_LINE_LENGTH];
    // Lewati header
    fgets(line, sizeof(line), file);
    
    while (fgets(line, sizeof(line), file)) {
        char *token = strtok(line, ",");
        int currentId = atoi(token);
        
        if (currentId == targetId) {
            doc.id = currentId;
            
            // Ambil nama
            token = strtok(NULL, ",");
            strncpy(doc.name, token, MAX_NAME_LENGTH - 1);
            doc.name[MAX_NAME_LENGTH - 1] = '\0';
            
            // Ambil max shifts per week
            token = strtok(NULL, ",");
            doc.maxShiftsPerWeek = atoi(token);
            
            // Ambil scheduled shifts
            for (int i = 0; i < MAX_WEEKS; i++) {
                token = strtok(NULL, ",");
                doc.scheduledShifts[i] = atoi(token);
            }
            
            // Ambil preferences
            for (int i = 0; i < DAYS_PER_WEEK; i++) {
                for (int j = 0; j < SHIFTS_PER_DAY; j++) {
                    token = strtok(NULL, ",");
                    doc.preferences[i][j] = atoi(token);
                }
            }
            break;
        }
    }
    
    fclose(file);
    return doc;
}

// Fungsi untuk menampilkan informasi dokter
void tampilkanInfoDokter(Doctor doc) {
    // Header
    printf("===============================================\n");
    printf("DOCTOR INFORMATION\n");
    printf("===============================================\n");
    
    // Informasi dasar
    printf("ID              : %d\n", doc.id);
    printf("Name            : %s\n", doc.name);
    printf("Max Shifts/Week : %d", doc.maxShiftsPerWeek);
    if (doc.maxShiftsPerWeek > 21) {
        printf(" [WARNING: Exceeds maximum possible (21)]");
    }
    printf("\n");
    
    // Status saat ini
    printf("Current Status  : ");
    for (int i = 0; i < MAX_WEEKS; i++) {
        printf("Week %d: %d", i + 1, doc.scheduledShifts[i]);
        if (i < MAX_WEEKS - 1) printf(", ");
    }
    printf("\n\n");
    
    // Tampilkan preferensi shift
    printf("SHIFT PREFERENCES:\n");
    printf("         Morning  Afternoon  Night\n");
    
    const char *days[] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
    for (int i = 0; i < DAYS_PER_WEEK; i++) {
        printf("%-9s", days[i]);
        for (int j = 0; j < SHIFTS_PER_DAY; j++) {
            printf("     %c    ", doc.preferences[i][j] ? 'Y' : 'N');
        }
        printf("\n");
    }
    printf("\n");
    
    // Hitung dan tampilkan ringkasan preferensi
    int totalSlots = DAYS_PER_WEEK * SHIFTS_PER_DAY;
    int availableSlots = 0;
    int unavailableSlots = 0;
    
    for (int i = 0; i < DAYS_PER_WEEK; i++) {
        for (int j = 0; j < SHIFTS_PER_DAY; j++) {
            if (doc.preferences[i][j]) availableSlots++;
            else unavailableSlots++;
        }
    }
    
    printf("PREFERENCE SUMMARY:\n");
    printf("- Available slots  : %d/%d (%.1f%%)\n", 
           availableSlots, totalSlots, (float)availableSlots/totalSlots * 100);
    printf("- Unavailable slots: %d/%d (%.1f%%)\n", 
           unavailableSlots, totalSlots, (float)unavailableSlots/totalSlots * 100);
    
    // Tampilkan periode blackout
    printf("- Blackout periods : ");
    int first = 1;
    const char *shifts[] = {"morning", "afternoon", "night"};
    
    for (int i = 0; i < DAYS_PER_WEEK; i++) {
        for (int j = 0; j < SHIFTS_PER_DAY; j++) {
            if (!doc.preferences[i][j]) {
                if (!first) printf(", ");
                printf("%s %s", days[i], shifts[j]);
                first = 0;
            }
        }
    }
    printf("\n");
    
    // Footer
    printf("===============================================\n");
}

int main() {
    int id;
    printf("Masukkan ID dokter: ");
    scanf("%d", &id);
    
    Doctor doc = bacaDataDokter("daftar_dokter.csv", id);
    
    if (doc.id == 0) {
        printf("Error: Dokter dengan ID %d tidak ditemukan\n", id);
        return 1;
    }
    
    tampilkanInfoDokter(doc);
    return 0;
}
