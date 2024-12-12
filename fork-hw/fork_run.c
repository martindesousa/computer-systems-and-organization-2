//Martin Desousa (crx7jg)
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

char *getoutput(const char *command)
{
    pid_t pid;
    int pipe_fd[2];
    char *output = NULL;
    size_t buffer = 0;


    if (pipe(pipe_fd) == -1)
    {
        return NULL;
    }

    pid = fork();

    if (pid == -1)
    {
        return NULL;
    }
    else if (pid == 0) //CHILD
    {
        dup2(pipe_fd[1], STDOUT_FILENO);
        close(pipe_fd[0]);
        close(pipe_fd[1]);

        execl("/bin/sh", "sh", "-c", command, (char *) NULL);
        _exit(127);
    } 
    else 
    {   //PARENT 

        close(pipe_fd[1]);
        FILE *file = fdopen(pipe_fd[0], "r");
        if (!file)
        {
            return NULL;
        }
        getdelim(&output, &buffer, '\0', file);
        close(pipe_fd[0]); 

        waitpid(pid, NULL, 0);
        return output;
    }

}

char *parallelgetoutput(int count, const char **argv_base)
{
    pid_t* pid_list;
    int pipe_fd[2];
    char *output = NULL;
    size_t output_size = 0;

    if (pipe(pipe_fd) == -1)
    {
        return NULL;
    }

    pid_list = malloc(count * sizeof(pid_t));

    for (int i = 0; i < count; i++)
    {
        pid_list[i] = fork();

        if (pid_list[i] == -1)
        {
            for (int j = 0; j < i; j++)
            {
                kill(pid_list[j], SIGTERM); 
            }
            close(pipe_fd[0]);
            close(pipe_fd[1]);
            free(pid_list);
            return NULL;
        }
        if (pid_list[i] == 0) //CHILD
        {
            close(pipe_fd[0]);

            int arg_count = 0;
            while (argv_base[arg_count] != NULL) {
                arg_count++;
            }

            const char **new_args = (const char **)malloc((arg_count + 2) * sizeof(const char *));
            for (int j = 0; j < arg_count; j++)
            {
                new_args[j] = argv_base[j];
            }
            char index_str[12];  
            snprintf(index_str, sizeof(index_str), "%d", i);  
            new_args[arg_count] = index_str;
            new_args[arg_count + 1] = NULL;

            dup2(pipe_fd[1], STDOUT_FILENO);
            close(pipe_fd[1]);

            execv(argv_base[0], (char *const *)new_args);
            _exit(127);
        }
    }
    close(pipe_fd[1]); //PARENT

    FILE *file = fdopen(pipe_fd[0], "r");
    if (!file)
    {
        return NULL;
    }
    char *line = NULL;
    size_t buffer = 0;
    ssize_t bytes_read;

    output = (char *)malloc(1); 
    output[0] = '\0';

    while ((bytes_read = getdelim(&line, &buffer, '\n', file)) != -1) {
        output_size += bytes_read;
        output = (char *)realloc(output, output_size + 1);
        strcat(output, line);
    }
    close(pipe_fd[0]);

    for (int i = 0; i < count; i++) {
        waitpid(pid_list[i], NULL, 0);
    }
    free(pid_list);
    free(line);
    return output;
}