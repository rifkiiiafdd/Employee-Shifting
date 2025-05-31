/*
spesifikasi : melakukan iterasi untuk mengisi jadwal dokter
input  : NULL
output : Jadwal Dokter yang seseuai ketentuan
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h> 

int number_doctor = 10;

int rng(int number_doctor) {
    srand(time(NULL));
    return rand() % number_doctor;
}

int main() {

    // array inizialitation
    int schedule[30][3] = {NULL};

    int random_doctor, id_doctor;
    for (int i = 0; i < 7; i++) {
        random_doctor = rng(number_doctor);
        int indeks = 0;
        
        
    }

    

    return 0;
}
