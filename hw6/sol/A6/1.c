#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

int counter = 0;
int n = 0;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

void *thread(void *arg)
{
    for (int i = 0; i < n; i++)
    {
        //decrease it thread safely
        pthread_mutex_lock(&lock);
        counter--;
        pthread_mutex_unlock(&lock);
    }
}

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        printf("usage: ./program a_number");
        return 1;
    }
    n = atoi(argv[1]);

    pthread_t tid_child;
    //create the child thread
    pthread_create(&tid_child, 0, thread, 0);
    for (int i = 0; i < n; i++)
    {
        pthread_mutex_lock(&lock);
        //increase it thread safely
        counter++;
        pthread_mutex_unlock(&lock);
    }
    //wait for the child thread.
    pthread_join(tid_child, 0);
    printf("counter:%d\n", counter);
    return 0;
}



/*Q: Tabulate your results by recording the values of n and counter in each experiment*/

/*
 *
 *      n=10,           counter=0
 *      n=100,          counter=0
 *      n=1000,         counter=0
 *      n=10000,        counter=0
 *      n=100000,       counter=0
 *      n=1000000,      counter=0
 *      n=10000000,     counter=0
 *      n=100000000,    counter=0
 *
*/

/* Q: Does the final printed value of the counter change from one experiment to the other, or is it constant? If it is constant, what is the value? Explain why if not constant.*/

/* The value of counter change is constant, the value is 0, since it is protected by the mutex lock，it cannot be modified by two threads at the same time. Now one is increased by n times, and the other is decreased by n times, so in the end, there is no increase or decrease*/
