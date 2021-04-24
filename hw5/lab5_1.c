#include <stdio.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <time.h>

#define READ_END 0
#define WRITE_END 1


// global var for fib
int prev1 = 1;
int prev2 = 1;
int times = 1;

int fib(void) {
    if (times == 1 || times == 2) {
        times++;
        return 1;
    }
    int next = prev1 + prev2;
    prev1 = prev2;
    prev2 = next;
    return next;
}

int main(int argc, char* argv[]) {
    int n = atoi(argv[1]);
    int fd[2];
    if(pipe(fd) == -1) {
        fprintf(stderr, "pipe failed");
        return 1;
    }
    pid_t pid = fork();
    if (pid == 0) {
        // producer
        close(fd[READ_END]);
        int result;
        srand( (unsigned)time(NULL));
        for (int i = 0; i < n; i++) {
            sleep(rand() % 2);
            result = fib();
            write(fd[WRITE_END], &result, sizeof(result));

        }
        close(fd[WRITE_END]);
    } else if (pid > 0) {
        // consumer
        close(fd[WRITE_END]);
        int result = 0;
        for (int i = 0; i < n; i++) {
            read(fd[READ_END], &result, sizeof(result));
            printf("%d: %d\n", i+1 , result);
        }
        close(fd[READ_END]);
        wait(NULL);

    } else {
        // redirect to standard error
        fprintf(stderr, "fork failed");
        return 1;
    }
}