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

    newnode->next = head;
    head = newnode;
    
    number_doctor += 1;

}

int rng(int number_doctor) {
    return rand() % number_doctor + 1;
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

    add(1, "Dummy", 4, preference);
    add(2, "Rena", 4, preference);
    add(3, "Levi", 5, preference);
    add(4, "Sinta", 4, preference);
    add(5, "Evelyn", 4, preference);




    // array inizialitation
    int schedule[30][3] ={0};

    int random_doctor, id_doctor;

    for (int i = 0; i < 30; i++) {

        for(int j = 0; j < 3; j ++){
        
        random_doctor = rng(number_doctor);
        // printf("\nrandom %d \n", random_doctor);

        int dokter_ke = 0;   
        int preference_focus = 1; // Jika bernilai 1, preferensi dokter diperhitungkan
        Doctor *temp = head;
        while(dokter_ke++ < number_doctor * 2) {
            // printf("Dokter ke %d  random %d  id %d jumlah dokter %d\n", dokter_ke , random_doctor, temp->id, number_doctor);
            if (temp->id == random_doctor) {
                // printf("i %d j %d\n",i + 1,j + 1);
                // printf("sift%d %d\n",temp->max_shift_per_week, temp->shift_scheduled_per_week[i / 7]);
                if (temp->max_shift_per_week > temp->shift_scheduled_per_week[i / 7]) {
                    
                    if (temp->preference[i%7][j] == 1 && preference_focus == 1) {
                        schedule[i][j] = temp->id;
                        temp->shift_scheduled_per_week[i / 7] += 1;
                        break;
                    }
                    else if (preference_focus == 0) {
                        schedule[i][j] = temp->id;
                        temp->shift_scheduled_per_week[i / 7] += 1;
                        break;
                    }
                }
                else {
                    random_doctor -= 1;
                    if (random_doctor == 0) {
                    random_doctor = number_doctor;
                    }
                }

                
            }

            temp = temp->next;
            

            if (temp == NULL) {
                temp = head;
            }

            // if (dokter_ke = number_doctor) {
            //     preference_focus = 0;
            // }
        }

        }
    }

    printing_matriks(30,3,schedule);

    

    return 0;
}
