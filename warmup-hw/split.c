#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "split.h"

char **string_split(const char *input, const char *sep, int *num_words)
{
    char* backup_input = strdup(input);
    char* current_char = backup_input;
    char** result_string = NULL;
    int count = 0;
    while (*current_char != '\0')
    {
        if (strspn(current_char, sep) > 0 && current_char == backup_input)
        {
            result_string = realloc(result_string, (count + 1) * sizeof(char *));
            result_string[count] = strdup("");  
            count++;
        }

        current_char += strspn(current_char, sep);

        if (*current_char == '\0') 
        {
            result_string = realloc(result_string, (count + 1) * sizeof(char *));
            result_string[count] = strdup("");  
            count++;
            break;
        }
    
        size_t current_word_len = strcspn(current_char, sep);
        result_string = realloc(result_string, (count + 1) * sizeof(char *));
        result_string[count] = (char *)malloc((current_word_len + 1) * sizeof(char));
        strncpy(result_string[count], current_char, current_word_len);
        result_string[count][current_word_len] = '\0';
        current_char += strcspn(current_char, sep);
        count++;
    }

    *num_words = count;
    free(backup_input);
    return result_string;



}