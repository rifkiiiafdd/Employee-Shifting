/*
spesifikasi : Membuat data dokter dalam bentuk struct dan memasukkannnya ke linked list
input  : nama dokter,id, preferensi shift (array), maksimal shift. 
output : struct doctor yang telah ditambahkan ke dalam linked list dokter

*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef  struct doctor {
int id;
char nama[20];
int max_shift;
int shift_total[5];
int preferensi_shift[7][3];
} doctor;


int main() {

    return 0;
}
