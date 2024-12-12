#include "lab.h"
#include <string.h>  // for memset
#include <stdio.h>   // for printf

#define NUM_MEASUREMENTS 10  


long calculate_median(long *times, int count) {
    for (int i = 0; i < count - 1; i++) {
        for (int j = i + 1; j < count; j++) {
            if (times[i] > times[j]) {
                long temp = times[i];
                times[i] = times[j];
                times[j] = temp;
            }
        }
    }
    return times[count / 2];
}

void find_passphrase(char *buffer, int length) {
    memset(buffer, 0, length + 1); //clear buffer
    int result;

    int i = 0; 
    while (1) {  
        char best_char = '\0';
        long best_time = 0;

        for (char c = 'a'; c <= 'z'; c++) {  // Test a-z
            buffer[i] = c;  // test current char

            long times[NUM_MEASUREMENTS];
            for (int m = 0; m < NUM_MEASUREMENTS; m++) {
                times[m] = measure_once(&result, buffer, check_passphrase);
            }

            long median_time = calculate_median(times, NUM_MEASUREMENTS);

            // If this is the best timing so far, update best_char and best_time
            if (median_time > best_time) {
                best_time = median_time;
                best_char = c;
            }

            if (result == 1) {  
                printf("Passphrase found: %s\n", buffer);
                return;  // Exit the function
            }
        }

        // Lock in the best guess for this character
        buffer[i] = best_char;
        printf("Locked in '%c' at position %d\n", best_char, i);

        i++;  // Next char

        if (i >= length) {
            i = 0;  
        }
    }
}