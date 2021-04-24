#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

int fork_process(int *counter, int n) {
    *counter = 0;
    int pid = fork();
    if (pid > 0) {    
        for (int i = 0; i < n; i++) {
            *counter = (*counter) + 1; //none atomic
//            __sync_fetch_and_add(counter, 1);
        }
        wait(NULL);
        printf("counter: %d\n", *counter);
        shm_unlink("shm");
    } else if (pid == 0) {
        for (int i = 0; i < n; i++) {
            *counter = (*counter) - 1; //none atomic
//            __sync_fetch_and_sub(counter, 1);
        }
    } else {
        printf("error");
    }
}


int main(int argc, char** argv) {
    // get "n" from command line
    int n = atoi(argv[1]);


    // create shared-mem named "shm"
    const char *name = "shm";
    const int SIZE = sizeof(int);
    int shm_fd;
    int *ptr;
    
    shm_fd = shm_open("shm", O_CREAT | O_RDWR, 0666);
    ftruncate(shm_fd, SIZE);
    ptr = mmap(0, SIZE, PROT_WRITE, MAP_SHARED, shm_fd, 0);

    // fork a child process to run
    fork_process(ptr, n);
    return 0;
}

