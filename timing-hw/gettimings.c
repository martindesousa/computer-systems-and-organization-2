#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/wait.h>
#include <string.h>

void start_timer(struct timespec *t) {
    clock_gettime(CLOCK_MONOTONIC, t);
}

double elapsed_time(struct timespec *start) {
    struct timespec elapsedt;
    clock_gettime(CLOCK_MONOTONIC, &elapsedt);
    return ((elapsedt.tv_sec - start->tv_sec) + ((elapsedt.tv_nsec - start->tv_nsec) / 1000000000.0));
}

void empty_func() {}

void sig_handler(int sig) {}

void check_func1() {
    int iterations = 100000;
    double total_time = 0.0;

    for (int i = 0; i < iterations; i++) {
        struct timespec t;
        start_timer(&t);
        empty_func();
        total_time += elapsed_time(&t);
    }

    double avg_elapsed_time = total_time / iterations;
    printf("Average time to run empty function over %d iterations: %0.9f seconds\n", iterations, avg_elapsed_time);
}

void check_func2() {
    int iterations = 100000;
    double total_time = 0.0;

    for (int i = 0; i < iterations; i++) {
        struct timespec t;
        start_timer(&t);
        getppid();
        total_time += elapsed_time(&t);
    }

    double avg_elapsed_time = total_time / iterations;
    printf("Average time to run getppid over %d iterations: %0.9f seconds\n", iterations, avg_elapsed_time);
}

void check_func3() {
    int iterations = 100;
    double total_time = 0.0;

    for (int i = 0; i < iterations; i++) {
        struct timespec t;
        start_timer(&t);
        system("/bin/true");
        total_time += elapsed_time(&t);
    }

    double avg_elapsed_time = total_time / iterations;
    printf("Average time to run system(\"/bin/true\") over %d iterations: %0.9f seconds\n", iterations, avg_elapsed_time);
}

void check_func4() {
    int iterations = 100000;
    double total_time = 0.0;

    signal(SIGUSR1, sig_handler);

    for (int i = 0; i < iterations; i++) {
        struct timespec t;
        start_timer(&t);
        raise(SIGUSR1);
        total_time += elapsed_time(&t);
    }

    double avg_elapsed_time = total_time / iterations;
    printf("Average time to send signal over %d iterations: %0.9f seconds\n", iterations, avg_elapsed_time);
}

void check_func5()
{
    pid_t other_pid;
    printf("Process PID: %d\n", getpid());
    printf("Enter the other process pid:");
    scanf("%d", &other_pid);

    sigset_t sigset;
    int sig;
    sigemptyset(&sigset);
    sigaddset(&sigset, SIGUSR1);
    sigprocmask(SIG_BLOCK, &sigset, NULL);

    struct timespec t;
    start_timer(&t);

    kill(other_pid, SIGUSR1);

    sigwait(&sigset, &sig);

    double avg_elapsed_time = elapsed_time(&t);
    printf("Time to send signal to other process and receive: %0.9f seconds\n", avg_elapsed_time);
}

void func_neg1() {
    pid_t other_pid;
    printf("Process PID: %d\n", getpid());
    printf("Enter the other process pid:");
    scanf("%d", &other_pid);

    sigset_t sigset;
    int sig;
    sigemptyset(&sigset);
    sigaddset(&sigset, SIGUSR1);
    sigprocmask(SIG_BLOCK, &sigset, NULL);

    sigwait(&sigset, &sig);
    kill(other_pid, SIGUSR1);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        return -1;
    }

    char* func_number = argv[1];

    if (strcmp(func_number, "1") == 0) {
        check_func1();
    } else if (strcmp(func_number, "2") == 0) {
        check_func2();
    } else if (strcmp(func_number, "3") == 0) {
        check_func3();
    } else if (strcmp(func_number, "4") == 0) {
        check_func4();
    } else if (strcmp(func_number, "5") == 0) {
        check_func5();
    } else if (strcmp(func_number, "-1") == 0) {
        func_neg1();
    } else {
        return -1;
    }

    return 0;
}
