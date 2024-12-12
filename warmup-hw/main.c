#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "split.h"


int main(int argc, char *argv[]) {
    char sep[4000];
    if (argc == 1) {
        strcpy(sep, " \t");  
    } else {
        sep[0] = '\0';  
        for (int i = 1; i < argc; i++) {
            strncat(sep, argv[i], 4000 - strlen(sep) - 1);
        }
    }

    char input[4000];
    while (fgets(input, 4000, stdin) != NULL) {

        size_t input_length = strlen(input);
        if (input[input_length - 1] == '\n') {
            input[input_length - 1] = '\0';
        }

        if (strcmp(input, ".") == 0) {
            break;
        }

        int num_words;
        char **words = string_split(input, sep, &num_words);

        for (int i = 0; i < num_words; i++) {
            printf("[%s]", words[i]);
            free(words[i]); 
        }
        printf("\n");

        free(words);
    }

    return 1;
}