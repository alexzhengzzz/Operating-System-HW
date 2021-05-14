#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <wait.h>


int main(int argc, char **argv)
{
    if (argc < 2)
    {
        printf("usage: ./program a_number\n");
        exit(EXIT_FAILURE);
    }
    int n = atoi(argv[1]);
    const int SIZE = sizeof(int);
    const char *name = "OS";

    int shm_fd;
    void *ptr;
    shm_fd = shm_open(name, O_CREAT | O_RDWR, 0666);
    if (shm_fd < 0)
    {
        printf("shm_open error\n");
        exit(EXIT_FAILURE);
    }
    ftruncate(shm_fd, SIZE);

    ptr = mmap(0, SIZE, PROT_WRITE, MAP_SHARED, shm_fd, 0);

    //initiate the shared memory with 0
    (*((int *)ptr)) = 0;

    pid_t child_pid = fork();
    if (child_pid < 0)
    {
        printf("fork error\n");
        exit(EXIT_FAILURE);
    }
    else if (child_pid == 0)
    {
        //decrease it
        for (int i = 0; i < n; i++)
        {
            (*((int *)ptr))--;
        }
        exit(0);
    }
    else
    {
        //increase it
        for (int i = 0; i < n; i++)
        {
            (*((int *)ptr))++;
        }
        int status = 0;
        wait(&status);
        printf("n:%-10d counter:%-10d\n", n, (*((int *)ptr)));
        shm_unlink(name);
    }

    return 0;
}


/*Q : Tabulate your results by recording the values of n and counter in each experiment*/
/*A:
/*
n:10         counter:0
n:100        counter:0
n:1000       counter:0
n:10000      counter:0
n:100000     counter:0
n:1000000    counter:-893766
n:10000000   counter:1933958
n:100000000  counter:-3078879
*/




/* Q : Does the final printed value of the counter change from one experiment to the other, or is it constant? If it is constant, what is the value? Explain why if not constant.*/

/* A: The final printed value of the counter change from one experiment to the other. Because the two processes are trying to modify the counter at the same time using non-atomic increemnts/decrements */




/*Q: Is there a range of values for n where the behavior is different from the behavior in other ranges? Explain why if any*/

/* A: There is a range of values for n where the behavior is different from the behavior in other ranges. When n is less than 1000000, the result is zero, but when n is more than 1000000, the result is not necessarily zero. When
    n> 1000000 The number of times that the parent-child processes operate the shared memory at the same time becomes more and more, which makes the result error.

