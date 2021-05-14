#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>
#include <string.h>
// the coding is just from the previous homework answer
// based on that code, I tried PTHREAD_SCOPE_PROCESS 
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
    int i, scope;
    pthread_attr_t attr;

    pthread_t tid_child; 
    pthread_attr_init( &attr );  
/* set the scheduling algorithm to PCS or SCS */ 
    errno=0;
    errno = pthread_attr_setscope(&attr, PTHREAD_SCOPE_PROCESS); 
    if(errno!=0)
    {
        printf("errno： %d \n",errno);
        printf("error msg： %s \n",strerror(errno));
    }    


 
if (pthread_attr_getscope(&attr, &scope) != 0) 
	fprintf(stderr, "Unable to get scheduling scope\n"); 
else { 
	if (scope == PTHREAD_SCOPE_PROCESS) 
	printf("PTHREAD_SCOPE_PROCESS\n"); 
	else if (scope == PTHREAD_SCOPE_SYSTEM) 
	printf("PTHREAD_SCOPE_SYSTEM\n"); 
	else
	fprintf(stderr, "Illegal scope value.\n"); 
} 

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
