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

typedef struct conflict {
    int id;
    char name[20];
    int hari;
    int shift;
    struct conflict* next;
} conflict;

conflict *head = NULL;

void conflict_schedule(int hari, int shift,int id, char *name) {
    conflict *newnode = malloc(sizeof(conflict));

    newnode ->hari = hari;
    newnode->shift = shift;
    newnode->id = id;
    strcpy(newnode->name, name);

    newnode->next = head;
    head = newnode;
}



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
    if (data[id - 1]->preference[day][shift] ==  1)  {
        return 1;
    }
    else {
        return 0;
    }

}

int main() {
    srand(time(NULL));

   int preference1[7][3] = {
    {1, 1, 1},
    {0, 0, 0},
    {1, 1, 1},
    {1, 1, 1},
    {1, 1, 1},
    {1, 1, 1},
    {1, 1, 0}
};

int preference2[7][3] = {
    {1, 1, 1},
    {1, 1, 1},
    {0, 1, 1},
    {0, 0, 0},
    {1, 1, 1},
    {1, 1, 1},
    {1, 1, 1}
};

int preference3[7][3] = {
    {0, 1, 1},
    {1, 1, 1},
    {1, 1, 1},
    {1, 1, 1},
    {1, 1, 1},
    {1, 1, 0},
    {0, 0, 0}
};

int preference4[7][3] = {
    {0, 0, 0},
    {0, 0, 0},
    {0, 0, 0},
    {1, 0, 0},
    {0, 1, 0},
    {0, 0, 1},
    {0, 0, 0}
};

int preference5[7][3] = {
    {0, 0, 0},
    {0, 0, 0},
    {0, 0, 0},
    {0, 0, 0},
    {1, 0, 0},
    {0, 1, 0},
    {0, 0, 1}
};

int preference6[7][3] = {
    {0, 0, 1},
    {0, 0, 0},
    {0, 0, 0},
    {0, 0, 0},
    {0, 0, 0},
    {1, 0, 0},
    {0, 1, 0}
};

int preference7[7][3] = {
    {0, 1, 0},
    {0, 0, 1},
    {0, 0, 0},
    {0, 0, 0},
    {0, 0, 0},
    {0, 0, 0},
    {1, 0, 0}
};



    // Data dokter
    add(1, "Dummy", 6, preference1);
    add(2, "Rena", 5, preference2);
    add(3, "Levi", 4, preference3);
    add(4, "Sinta", 4, preference2);
    add(5, "Evelyn", 5, preference3);
    // add(6, "Albert", 3, preference6);
    // add(7, "Tony", 3, preference7);


    // array inizialitation
    int schedule[30][3] ={0};

    int random_doctor, id_doctor;
    int preference_focus = 1;

    for (int i = 0; i < 30; i++) {
        
        // if (i % 7 == 1 && i != 1) {
        //         preference_focus = 1;
        //     }

        for(int j = 0; j < 3; j ++){
        preference_focus = 1;
        
        random_doctor = rng(number_doctor);
        // printf("\nrandom %d \n", random_doctor);

        int dokter_ke = 0;   
        Doctor *temp = data[random_doctor];
        // printf("i %d j %d fre %d\n",i+1,j+1, preference_focus);
        while(dokter_ke++ < number_doctor*2) {

            // printf("i %d j %d\n",i + 1,j + 1);
            // printf("Dokter ke %d  random %d  id %d jumlah dokter %d, preference focus %d bisa ngga %d\n", dokter_ke , random_doctor, temp->id, number_doctor, preference_focus, temp->preference[i % 7][j]);
            // printf("shift max %d, shfit sekarang %d\n", temp->max_shift_per_week , temp->shift_scheduled_per_week[i / 7]);

            if (temp->max_shift_per_week > temp->shift_scheduled_per_week[i / 7]) {

                if (temp->preference[i % 7][j] == 1 && preference_focus == 1) {
                    // printf("Hahaha\n");
                    
                    schedule[i][j] = temp->id;
                    temp->shift_scheduled_per_week[i / 7] += 1;
                    dokter_ke = 0;
                    break;
                }
                else if (preference_focus == 0){
                    // printf("haduh\n");
                    schedule[i][j] = temp->id;
                    temp->shift_scheduled_per_week[i / 7] += 1;
                    break;
                } 

                

            }

            if (dokter_ke > number_doctor - 1) {
                preference_focus = 0;
            }
            
            random_doctor += 1;

            if (random_doctor  == number_doctor) {
                random_doctor = 0;
            }
            temp = data[random_doctor];

                
            

            
        }
        }
    }

    // printf("Halo dek\n");
    // printing_matriks(30,3,schedule);

    // cek tabrakan
    int id_awal, id_pengganti, alternatif_found;
    for (int i = 0; i < 30; i ++) {
        for (int j = 0; j < 3; j ++) {
            if (check_preference(schedule[i][j], i%7,j) == 0) {
                id_awal = schedule[i][j];
                alternatif_found = 0;
                // printf("Nabrak Njir. hari : %d  sesi %d\n", i+1, j + 1);
                for(int k = i - (i % 7); k < i - (i % 7) + 7; k ++) {
                    for (int l = 0; l < 3; l++) {
                        id_pengganti = schedule[k][l];
                        // printf("k : %d l : %d\n", k+1, l+1);
                        // printf("masalah : %d, bisa ganti %d, pengganti %d, bisa ganti?%d \n", id_awal, check_preference(id_awal, k%7,l), id_pengganti, check_preference(id_pengganti, i%7, j));
                        if (check_preference(id_awal, k%7,l)  && check_preference(id_pengganti, i%7, j)) {
                            schedule[i][j] = id_pengganti;
                            schedule[k][l] = id_awal;
                            alternatif_found = 1;
                            break;
                        }

                        
                    }
                    if (alternatif_found == 1) {
                        break;
                    }
                }

            }
        }
    }


    for (int i = 0; i < 30; i ++) {
        for (int j = 0; j < 3; j ++) {
            if (check_preference(schedule[i][j], i%7,j) == 0) {
                conflict_schedule(i+1,j+1, schedule[i][j], data[schedule[i][j] - 1]->name);
                
            }
        }
    }


    printing_matriks(30,3,schedule);
    printf("\n");

    conflict *temp = head;
    while (temp != NULL) {
        printf("id : %d\n", temp->id);
        printf("Hari : %d Shift : %d\n\n", temp->hari, temp->shift);
        temp = temp->next;
    }
    
    return 0;
}
