#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/mman.h>
#include <errno.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>
#include <sys/types.h>
#include <semaphore.h>


#define QUEUE_SIZE (7 * sizeof(int))
#define FILE_SIZE (QUEUE_SIZE + sizeof(sem_t) * 2)
#define SHARED_MEMORY_NAME "shared_memory"


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
    /**
     * there are two sem_t:
     *      sem_array[0]: empty_count
     *      sem_array[1]: filled_count
    */
    sem_t *sem_array = (sem_t *)(((char *)arr_fib) + QUEUE_SIZE);
    sem_t *empty_count = sem_array;
    sem_t *filled_count = sem_array + 1;
    sem_init(empty_count, 1, 5); //the queue has at most 5 empty elements.
    sem_init(filled_count, 1, 0);//initially, there is no filled count

    int *in = arr_fib;
    int *out = arr_fib + 1;
    *in = *out = 0;
    arr_fib = arr_fib + 2;
    if (fork() == 0)
    {
        srand(time(NULL));
        for (int i = 0; i < n; i++)
        {
            usleep(rand() % 1000000);
            sem_wait(empty_count);
            arr_fib[*in] = Fibonacci();
            *in = (*in + 1) % 5;
            sem_post(filled_count);
        }
        exit(0);
    }
    else
    {
        printf("values:\n");
        for (int i = 0; i < n; i++)
        {
            sem_wait(filled_count);
            int v = arr_fib[*out];
            *out = (*out + 1) % 5;
            sem_post(empty_count);
            printf(" %d \n", v);
        }
        printf("\n");
        sem_destroy(empty_count);
        sem_destroy(filled_count);
        int status;
        wait(&status);
        shm_unlink("shared_memory");
    }
    return 0;
}

