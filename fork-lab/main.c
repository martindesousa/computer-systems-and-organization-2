#include <stdio.h>
#include <sys/wait.h>  
#include <unistd.h>     
#include <stdlib.h>    

int my_system(const char *command);

int main() {
    printf("Running blocking commands...\n");
    int a1 = my_system("sleep 1; echo hi");
    int a2 = my_system("echo bye");
    int a3 = my_system("flibbertigibbet 23");

    printf("%d %d %d\n", WEXITSTATUS(a1), WEXITSTATUS(a2), WEXITSTATUS(a3));

    my_system("echo -n 'type something: ';"
        " read got;"
        " echo Thanks for typing \\\"\"$got\"\\\"");
        
    return 0;
}
