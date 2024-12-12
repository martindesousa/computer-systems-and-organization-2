#define _GNU_SOURCE
#include "util.h"
#include <stdio.h>      // for printf
#include <stdlib.h>     // for atoi (and malloc() which you'll likely use)
#include <sys/mman.h>   // for mmap() which you'll likely use
#include <stdalign.h>

alignas(4096) volatile char global_array[4096 * 32];

void labStuff(int which) {
    if (which == 0) {
        /* do nothing */
    } else if (which == 1) {
        for (int i = 0; i < 32; i++) {
            global_array[i * 4096] = (char)1; 
        }
        for (int i = 0; i < 100; i++) {
            global_array[0] = (char)1; 
        }
    } else if (which == 2) {
        char* mb_memory = malloc(1000000);
        mb_memory[1] = (char)2;
        mb_memory[4099] = (char)2;
        mb_memory[10000] = (char)2;
        mb_memory[16000] = (char)2;
    } else if (which == 3) {
        char *memory;
        memory = mmap(NULL, 1048576, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        if (memory == MAP_FAILED) { return; }
        for (int i = 0; i < 32; i++) {
            memory[i * 4096] = (char)3; 
        }

    } else if (which == 4) {
        char *memory;
        memory = mmap((void*)(0x5555555bc000 + 0x200000), 4096, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED_NOREPLACE, -1, 0);
        if (memory == MAP_FAILED) { return; }

        for (int i = 0; i < 4096; i++)
        {
            memory[i] = (char)4;
        }
    } else if (which == 5) {
        char *memory;
        memory = mmap((void*)(0x5555555bc000 + 0x10000000000), 4096, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED_NOREPLACE, -1, 0);
        if (memory == MAP_FAILED) { return; }

        for (int i = 0; i < 4096; i++)
        {
            memory[i] = (char)5;
        }
    }

}

int main(int argc, char **argv) {
    int which = 0;
    if (argc > 1) {
        which = atoi(argv[1]);
    } else {
        fprintf(stderr, "Usage: %s NUMBER\n", argv[0]);
        return 1;
    }
    printf("Memory layout:\n");
    print_maps(stdout);
    printf("\n");
    printf("Initial state:\n");
    force_load();
    struct memory_record r1, r2;
    record_memory_record(&r1);
    print_memory_record(stdout, NULL, &r1);
    printf("---\n");

    printf("Running labStuff(%d)...\n", which);

    labStuff(which);

    printf("---\n");
    printf("Afterwards:\n");
    record_memory_record(&r2);
    print_memory_record(stdout, &r1, &r2);
    print_maps(stdout);
    return 0;
}
