#include <stdio.h>
#include <stdlib.h>

char *getoutput(const char *command);
char *parallelgetoutput(int count, const char **argv_base);

int main() {
    // Example usage of getoutput
    printf("Testing getoutput:\n");
    char *output = getoutput("echo 1 2 3; sleep 2; echo 5 5");
    printf("Output: [[[%s]]]\n", output);
    free(output); // Free the memory allocated by getoutput

    // Example usage of parallelgetoutput
    printf("\nTesting parallelgetoutput:\n");
    const char *argv_base[] = {"/bin/echo", "running", NULL};
    char *parallel_output = parallelgetoutput(2, argv_base);
    printf("Text: [%s]\n", parallel_output);
    free(parallel_output); // Free the memory allocated by parallelgetoutput

    return 0;
}
