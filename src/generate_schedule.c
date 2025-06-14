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
    // Penyimpanan data dokter
    int id;
    char name[20];  
    int max_shift_per_week;
    int shift_scheduled_per_week[5];
    int preference[7][3];
} Doctor;

typedef struct conflict {
    // Pernyimpan data jadwal dokter yang preferensinya tidak terpenuhi
    int id;
    char name[20];
    int hari;
    int shift;
    struct conflict* next;
} conflict;

conflict *head = NULL; // head linked list jadwal konflik


/*
Menambahkan data dokter ke Array of struct
*/

Doctor* data[10]; 

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

/*
Menambahkan data jadwal yang tabrakan ke linked list tabrakan
*/
void conflict_schedule(int hari, int shift,int id, char *name) {
    conflict *newnode = malloc(sizeof(conflict));

    newnode ->hari = hari;
    newnode->shift = shift;
    newnode->id = id;
    strcpy(newnode->name, name);

    newnode->next = head;
    head = newnode;
}


// inisialisasi array penyimpan data dokter


/*
Mencetak data yang disimpan dalam bentuk matriks
*/
void printing_matriks(int baris, int kolom, int matriks[baris][kolom]) 
{
    // Menunjukkan isi matriks
    for (int i = 0; i < baris; i++)
    {
        printf("%d    ", i + 1);
        for (int j = 0; j < kolom;j++)
        {
            printf("[%d]%s     ",matriks[i][j], data[matriks[i][j] - 1]->name);
        }
        printf("\n");
    }
}


/*
Mencari angka random pada rentang 0 sampai jumlah doktor yang ada
*/
int rng(int number_doctor) {
    return rand() % number_doctor;
}


/*
Mengecek Preferensi. Jika tidak bisa pada shift tersebut, memberikan output 1
*/
int check_preference(int id, int day, int shift) {
    if (data[id - 1]->preference[day][shift] ==  1)  {
        return 1;
    }
    else {
        return 0;
    }

}


int main() {
    srand(time(NULL)); // membuat output fungsi rng benar benar random meskipun dijalankan berulang kali pada suatu iterasi

    /*
    data preferensi dokter
    jika 0 berarti tidak bisa pada shift tersebut, jika 1 bisa
    baris adalah hari ke-..
    kolom adalah shift ke-...
    */
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
    {1, 1, 0}
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
    add(1, "Doni", 6, preference1);
    add(2, "Rena", 5, preference2);
    add(3, "Levi", 4, preference3);
    add(4, "Alvi", 4, preference2);
    add(5, "Vely", 5, preference3);


    // inisialisasi matriks berdimensi 30 x 3 yang meyimpan id dokter yang bertugas pada shift tersebut
    // baris : tanggal
    // kolom : sesi
    int schedule[30][3] ={0};

    int random_doctor, id_doctor;

    
    // boolean yang menunjukkan apakah preferensi dokter diperhitungkan. Jika 1, maka preferensi diperhitungkan. jika 0, hanya shift maksimal yang diperhitungkan
    int preference_focus = 1; 

    /*
    iterasi per sesi per hari untuk 3 sesi per hari selama 30 hari.
    */
    for (int i = 0; i < 30; i++) {

        for(int j = 0; j < 3; j ++){
        preference_focus = 1; // di awal pencarian dokter, preferensi difokuskan
        
        random_doctor = rng(number_doctor);

        int dokter_ke = 0;   
        //  dokter_ke adalah counter yang menghitung berapa dokter yang dicek kesediannya.

        Doctor *temp = data[random_doctor];
        // mengambiil data dokter secara random

        // printf("i %d j %d fre %d\n",i+1,j+1, preference_focus);
        while(dokter_ke++ < number_doctor*2) {

            // printf("i %d j %d\n",i + 1,j + 1);
            // printf("Dokter ke %d  random %d  id %d jumlah dokter %d, preference focus %d bisa ngga %d\n", dokter_ke , random_doctor, temp->id, number_doctor, preference_focus, temp->preference[i % 7][j]);
            // printf("shift max %d, shfit sekarang %d\n", temp->max_shift_per_week , temp->shift_scheduled_per_week[i / 7]);

            if (temp->max_shift_per_week > temp->shift_scheduled_per_week[i / 7]) {
                // Mengecek apakah shift yang dimiliki dokter telah maksimal

                if (temp->preference[i % 7][j] == 1 && preference_focus == 1) {
                    // Mengecek apakah jadwal pada shift ini bertentangan dengan preferensi dokter
                    

                    schedule[i][j] = temp->id; // assgin data dokter yang memenuhi
                    temp->shift_scheduled_per_week[i / 7] += 1; // menambah jumlah shift yang telah diambil dokter
                    break;
                }
                else if (preference_focus == 0){

                    schedule[i][j] = temp->id;
                    temp->shift_scheduled_per_week[i / 7] += 1;
                    break;
                }      

            }

            if (dokter_ke > number_doctor - 1) {
                // apabila semua dokter telah dicek dan tidak ada yang bisa, preferensi dokter diabaikan
                preference_focus = 0;
            }
            
            random_doctor += 1; // setiap kali dokter yang dicek tidak memenuhi, dicek dokter selanjutnya

            if (random_doctor  == number_doctor) {
                random_doctor = 0;
                // jika telah sampai di ujung data, kembali ke awal
            }

            temp = data[random_doctor];
      
        }
        }
    }


    // pengganti jadwal
    int id_awal, id_pengganti, alternatif_found;
    // melakukan iterasi untuk mengecek apakah dokter di shift  tersebut tidak sesuai preferensinya
    for (int i = 0; i < 30; i ++) {
        for (int j = 0; j < 3; j ++) {


            if (check_preference(schedule[i][j], i%7,j) == 0) {
                id_awal = schedule[i][j]; // id dokter yang bermasalah

                alternatif_found = 0;// boolean yang menunjukkan apakah dokter pengganti ditemukan

                // printf("Nabrak Njir. hari : %d  sesi %d\n", i+1, j + 1);

                // iterasi yang dimulai pada hari pertama minggu tersebut
                for(int k = i - (i % 7); k < i - (i % 7) + 7; k ++) {
                    for (int l = 0; l < 3; l++) {
                        id_pengganti = schedule[k][l]; // id kandidat pengganti

                        // printf("k : %d l : %d\n", k+1, l+1);
                        // printf("masalah : %d, bisa ganti %d, pengganti %d, bisa ganti?%d \n", id_awal, check_preference(id_awal, k%7,l), id_pengganti, check_preference(id_pengganti, i%7, j));


                        if (check_preference(id_awal, k%7,l)  && check_preference(id_pengganti, i%7, j)) {
                            // mengecek apakah kedua dokter itu bisa saling menggantikan. jika bisa , id dalam jadwal ditukar
                            schedule[i][j] = id_pengganti;
                            schedule[k][l] = id_awal;
                            alternatif_found = 1;

                            // menghentikan proses saat ditemukan pengganti
                            break;
                        }

                        
                    }
                    if (alternatif_found == 1) {
                        // menghentikan proses saat pengganti ditemukan
                        break;
                    }
                }

            }
        }
    }


    // Mendata tabrakan preferensi yang terpaksa dilakukan
    for (int i = 0; i < 30; i ++) {
        for (int j = 0; j < 3; j ++) {
            if (check_preference(schedule[i][j], i%7,j) == 0) {
                conflict_schedule(i+1,j+1, schedule[i][j], data[schedule[i][j] - 1]->name);
                // data ditambahkan ke linked list
            }
        }
    }

    // mencetak data
    printing_matriks(30,3,schedule);
    printf("\n");


    // menampilkan tabrakan preferensi dokter
    conflict *temp = head;
    while (temp != NULL) {
        printf("id : %d\n", temp->id);
        printf("Hari : %d Shift : %d\n\n", temp->hari, temp->shift);
        temp = temp->next;
    }
    
    return 0;
}
