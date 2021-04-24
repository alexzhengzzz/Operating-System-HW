#include <unistd.h>
#include <sys/ipc.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <string.h>
#include <wait.h>
#include <time.h>
#include <fcntl.h>
#include <semaphore.h>
#include <sys/mman.h>

#define FILE_SIZE (7 * sizeof(int))
#define SHARED_MEMORY_NAME "shared_memory"

// modify the hw4 solution add the semaphore to avoid spinlock
int Fibonacci()
{
    static int i = 0, a = 1, b = 1;
    i++;
    if (i == 1 || i == 2)
        return 1;
    int a1 = a;
    int b1 = b;
    a = b;
    b = a1 + b1;
    return a1 + b1;
}
const char* full_name = "full1";
const char* empty_name = "empty1";
const char* mutex_name = "mutex1";

int main(int argc, char **argv)
{
    if (argc <= 1)
        return 1;
    int n = atoi(*(argv + 1));
    int status;
    int key = 1111;
    int shmid;
    int *arr_fib;

    shmid = shm_open(SHARED_MEMORY_NAME, O_CREAT | O_RDWR, 0777);
    if (-1 == shmid)
    {
        perror("shm  failed: ");
        exit(1);
    }
    ftruncate(shmid, FILE_SIZE);
    arr_fib = (int *)mmap(NULL, FILE_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shmid, SEEK_SET);
    if (arr_fib == (void *)-1)
    {
        printf("connect to share memory failed: %s\n", strerror(errno));
        exit(1);
    }
    int *in = arr_fib;
    int *out = arr_fib + 1;
    *in = *out = 0;
    arr_fib = arr_fib + 2;

    sem_t *full = sem_open(full_name, O_CREAT, 0666, 0);
    sem_t *empty = sem_open(empty_name, O_CREAT, 0666, 5);
    sem_t *mutex = sem_open(mutex_name, O_CREAT, 0666, 1);

    //printf("full: %d\n", *((int *)full));
    //printf("empty: %d\n", *((int *)empty));
    //printf("mutex: %d\n", *((int *)mutex));


    if (fork() == 0)
    {
        srand(time(NULL));
        for (int i = 0; i < n; i++)
        {
            sem_wait(empty);
            sem_wait(mutex);
            usleep(rand() % 1000000);
            //if (((*in + 1) % 5) == *out);
            arr_fib[*in] = Fibonacci();
            *in = (*in + 1) % 5;
            sem_post(mutex);
            sem_post(full);
        }
        exit(0);
    }
    else
    {
        printf("values:\n");
        for (int i = 0; i < n; i++)
        {
            sem_wait(full);
            sem_wait(mutex);
            //if (*in == *out)
            //    ;
            int v = arr_fib[*out];
            *out = (*out + 1) % 5;
            printf(" %d \n", v);
            sem_post(mutex);
            sem_post(empty);
        }
        printf("\n");
        int status;
        wait(&status);
        shm_unlink("shared_memory");
        sem_unlink(full_name);
        sem_unlink(empty_name);
        sem_unlink(mutex_name);
    }
    return 0;
}
