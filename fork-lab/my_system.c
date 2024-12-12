#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

int my_system(char *input_command)
{
    pid_t pid;
    int status;

    pid = fork();

    if (pid == -1)
    {
        return -1;
    }
    else if (pid == 0)
    {
        execl("/bin/sh", "sh", "-c", input_command, (char *) NULL);
        _exit(127);
    }
    else 
    {
        if (waitpid(pid, &status, 0))
        {
            return status;
        }
        return -1;
    }
}