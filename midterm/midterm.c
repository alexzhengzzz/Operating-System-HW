#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <time.h>
#include <sys/shm.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <math.h>
#include <semaphore.h>
#include <wait.h>

#define SHARED_MEMORY_NAME "shared_memory"
#define SIZE (1 * sizeof(int))
#define NUM  1000000

// command to run this code
// gcc midterm.c -lrt -pthread -lm -o midterm.out
// ./midterm.out

int isInCircle(double x, double y) {
    if (sqrt(x * x + y * y) <= 1.0) {
        return 1;
    }
    return 0;
}


int main( )
{
    int shmid;
    int *ptr;
    shmid = shm_open(SHARED_MEMORY_NAME, O_CREAT | O_RDWR, 0777);
    if (-1 == shmid)
    {
        perror("shm failed: ");
        exit(1);
    }
    ftruncate(shmid, SIZE);
    ptr = (int *)mmap(NULL, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shmid, SEEK_SET);

    // init counter
    int *counter = ptr;
    *counter = 0;
    
    // binary semaphore
    const char* mutex_name = "/mutex";
    sem_t *mutex = sem_open(mutex_name, O_CREAT, 0666, 1);

    pid_t child_pid = fork();
    if (child_pid < 0)
    {
        printf("fork error\n");
        exit(EXIT_FAILURE);
    }
    else if (child_pid == 0)
    {
        srand((unsigned)time(NULL));
        for (int i = 0; i < NUM; i++) {
            double x = 2 * (double)rand() / RAND_MAX - 1;
            double y = 2 * (double)rand() / RAND_MAX - 1;
            int result = isInCircle(x, y);
            sem_wait(mutex);
            if (result) {
                *counter = *counter + 1;
            }
            sem_post(mutex);
        }
        exit(0);
    }
    else
    {
        srand((unsigned)time(NULL));
        for (int i = 0; i < NUM; i++) {
            double x = 2 * (double)rand() / RAND_MAX - 1;
            double y = 2 * (double)rand() / RAND_MAX - 1;
            sem_wait(mutex);
            int result = isInCircle(x, y);
            if (result) {
                *counter = *counter + 1;
            }
            sem_post(mutex);
        }
        int status = 0;
        wait(&status);
        double total = 2000000.00;
        double area = 4.00 * (double)(*counter) / total;
        printf("area of circle is %lf", area);
        shm_unlink(SHARED_MEMORY_NAME);
        sem_unlink(mutex_name);
    }
    return 0;
}
