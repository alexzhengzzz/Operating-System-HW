#include <stdio.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

// sync problem happens when n = 100000
int n = 0;
pthread_mutex_t mut;

// thread routine
void* my_thread(void* counter){
    pthread_mutex_lock(&mut);
    int *p = (int *) counter;
    for (int i = 0; i < n; i++) {
        *p = *p - 1;
    }
    pthread_mutex_unlock(&mut);
    // Return reference to global variable:
    pthread_exit(NULL);
}

int main(int argc, char* argv[]) {
    n = atoi(argv[1]);
    // shared var
    int counter = 0;

    // Declare variable for thread's ID:
    pthread_t id;

    // id priority routine share var
    pthread_mutex_init(&mut, NULL);

    // create my thread
    pthread_create(&id, NULL, my_thread, &counter);

    // main thread
    pthread_mutex_lock(&mut);
    for (int i = 0; i < n; i++) {
        counter++;
    }
    pthread_mutex_unlock(&mut);

    // wait for my thread
    pthread_join(id, NULL);
    printf("counter = %d\n", counter);
    pthread_mutex_destroy(&mut);

}







