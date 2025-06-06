/*
spesifikasi : melakukan iterasi untuk mengisi jadwal dokter
input  : NULL
output : Jadwal Dokter yang seseuai ketentuan
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h> 

int number_doctor = 0;

typedef  struct Doctor {
    int id;
    char name[20];
    int max_shift_per_week;
    int shift_scheduled_per_week[5];
    int preference[7][3];
    struct Doctor *next;
} Doctor;

Doctor *head = NULL;
Doctor* data[10];

void printing_matriks(int baris, int kolom, int matriks[baris][kolom]) 
{
    // Menunjukkan isi matriks
    for (int i = 0; i < baris; i++)
    {
        for (int j = 0; j < kolom;j++)
        {
            printf("%d ", matriks[i][j]);
        }
        printf("\n");
    }
}

void add(int id, char* name, int max_shift, int preference[7][3]) {
    Doctor* newnode = malloc(sizeof(Doctor));

    
    strcpy(newnode->name, name);

    newnode->id = id;
    newnode->max_shift_per_week = max_shift;
    
    for (int i = 0; i < 7; i ++) {
        for (int j = 0 ; j < 3; j ++) {
            newnode->preference[i][j] = preference[i][j];
        }
    }
    
    for (int i = 0; i < 5 ; i ++ ){
        newnode->shift_scheduled_per_week[i] = 0;
    }

    data[number_doctor] = newnode;

    number_doctor += 1;

}

int rng(int number_doctor) {
    return rand() % number_doctor;
}


int check_preference(int id, int day, int shift) {
    // Mengecek Preferensi. Jika tidak bisa pada shift tersebut. maka output 1
    if (data[id - 1]->preference[day][shift])  {
        return 1;
    }
    else {
        return 0;
    }

}

int main() {
    srand(time(NULL));

    int preference[7][3] = {
        {1,1,1},
        {1,1,1},
        {1,1,1},
        {1,1,1},   
        {1,1,1},
        {1,1,1},
        {1,1,1},
    };
    int preference2[7][3] = {
        {1,1,1},
        {1,1,1},
        {1,1,1},
        {1,1,1},   
        {1,1,1},
        {0,0,0},
        {1,1,1},
    };

    add(1, "Dummy", 18, preference2);
    add(2, "Rena", 3, preference);
    // add(3, "Levi", 5, preference);
    // add(4, "Sinta", 5, preference);
    // add(5, "Evelyn", 5, preference);


    // array inizialitation
    int schedule[30][3] ={0};

    int random_doctor, id_doctor;

    for (int i = 0; i < 30; i++) {

        for(int j = 0; j < 3; j ++){
        
        random_doctor = rng(number_doctor);
        // printf("\nrandom %d \n", random_doctor);

        int dokter_ke = 0;   
        Doctor *temp = data[random_doctor];
        while(dokter_ke++ < number_doctor * 2) {

            if (temp->max_shift_per_week > temp->shift_scheduled_per_week[i / 7]) {
                schedule[i][j] = temp->id;
                temp->shift_scheduled_per_week[i / 7] += 1;
                break;
            }

            else {
                random_doctor += 1;
                if (random_doctor  == number_doctor) {
                    random_doctor = 0;
                }
                temp = data[random_doctor];
            }

        }
        }
    }

    

    // program penukar jadwal
    int id_awal, id_pengganti;
    for (int i = 0; i < 7; i++) {
        for (int j = 0; j < 3; j++) {
            id_awal = schedule[i][j];
            if (check_preference(id_awal, i, j) == 0) {
                for (int k = 0; k < 7; k++) {
                    for (int l = 0; l < 3; l++) {
                        id_pengganti =  schedule[k][l];

                        if (id_pengganti != id_awal) {
                            if (check_preference(id_pengganti, i, j) == 1 && check_preference(id_awal, k, l) == 1) {
                            schedule[i][j] = id_pengganti;
                            schedule[k][l] = id_awal;
                            }
                            break;
                        }
                    }
                    if (schedule[i][j] != id_awal) {
                        break;
                    }
                }
            }
        }
    }
    
    printing_matriks(7,3,schedule);
    

    return 0;
}
