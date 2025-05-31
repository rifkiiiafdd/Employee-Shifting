#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int rng(int number_doctor) {
    srand(time(NULL));
    return rand() % number_doctor;
}

int main () {
   printf("%d", rng(10));
}