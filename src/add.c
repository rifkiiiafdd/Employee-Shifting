/*
spesifikasi : Membuat data dokter dalam bentuk struct dan memasukkannnya ke linked list
input  : nama dokter,id, preferensi shift (array), maksimal shift. 
output : struct doctor yang telah ditambahkan ke dalam Array of Structurs dokter

*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef  struct Doctor {
    // Penyimpanan data dokter
    int id;
    char name[20];  
    int max_shift_per_week;
    int shift_scheduled_per_week[5];
    int preference[7][3];
} Doctor;

int number_doctor = 0;

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

int main() {


    return 0;
}
