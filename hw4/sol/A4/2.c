#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/mman.h>
#include <errno.h>
#include <wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>
#include <sys/types.h>

#define FILE_SIZE (7 * sizeof(int))
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
            while (((*in + 1) % 5) == *out);
            arr_fib[*in] = Fibonacci();
            *in = (*in + 1) % 5;
        }
        exit(0);
    }
    else
    {
        printf("values:\n");
        for (int i = 0; i < n; i++)
        {
            while (*in == *out)
                ;
            int v = arr_fib[*out];
            *out = (*out + 1) % 5;
            printf(" %d \n", v);
        }
        printf("\n");
        int status;
        wait(&status);
        shm_unlink("shared_memory");
    }
    return 0;
}

/* Q: Using you implementation (as described in slides 5-8), what’s the maximum number of elements theshared buffer can actually hold? Why?*/

/* A: The maximum numbr of elements the shared buffer can actually hold is 4. By described in slides 5-8, we can only use N-1 elements when using the cycle list to represent a queue. So if we create a buffer with 5 size, we can only hold 4 elements maximum.*/
