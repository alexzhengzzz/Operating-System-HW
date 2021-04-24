#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <time.h>

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
    const int FLAG = -1;
    // create shared buffer
    const char *name = "shm";
    const int SIZE = 5 * sizeof(int);
    int shm_fd = shm_open(name, O_CREAT | O_RDWR, 0666);
    ftruncate(shm_fd, SIZE);

    // init
    int *init = mmap(0, SIZE, PROT_WRITE, MAP_SHARED, shm_fd, 0);
    for (int i = 0; i <5; i++) {
        init[i] = FLAG;
    }

    int pid = fork();
    if (pid == 0) {
        // producer
        int *producer = mmap(0, SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, shm_fd, 0);
        int result;
        int index;
        srand( (unsigned)time(NULL));
        for (int i = 0; i < n; i++) {
            sleep(rand() % 2);
            result = fib();
            index = i % 5;
            while(producer[index] != FLAG);
            producer[index] = result;
        }
    } else if (pid > 0) {
        // consumer
        int *consumer = mmap(0, SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, shm_fd, 0);
        int index;
        for (int i = 0; i < n; i++) {
            index = i % 5;
            while (consumer[index] == FLAG);
            printf("%d: %d\n", i+1 , consumer[index]);
            consumer[index] = FLAG;
        }
        wait(NULL);
        shm_unlink(name);
    } else {
        // redirect to standard error
        fprintf(stderr, "fork failed");
    }
}