/* -*- compile-command: "gcc -Wall -pedantic -g3 ht_main.c -o ht_main" -*- */
#include <stdio.h>
#include <stdlib.h>
#include "hash_table.h"

int main(int argc, char *argv[]) {

    ht_hash_table* ht = ht_new();
    ht_del_hash_table(ht);

    return EXIT_SUCCESS;
}
