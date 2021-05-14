#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <pthread.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

// enter the "bye" in both sides will exit 
char fifo_0[] = "/tmp/fifo_0";
char fifo_1[] = "/tmp/fifo_1";
char exit_str[] = "bye";
int inst;
int flag = 1;
void *send_thread(void *arg)
{
    int fd;
    if (inst == 0) {
        fd = open(fifo_0, O_WRONLY);
    }
    if (inst == 1) {
        fd = open(fifo_1, O_WRONLY);
    }
    if (fd == -1)
        printf("Open FIFO error!\n");

    char input[256];
    while (flag == 1){
        fgets(input, 255, stdin);
        write(fd, input, 255);
        // exit
        if (strncmp(exit_str, input, 3) == 0) {
            flag = 0;
            close(fd);
            pthread_exit(NULL);
        }

    }
}

void *recv_thread(void *arg)
{
    int fd;
    if (inst == 0) {
        fd = open(fifo_1, O_RDONLY);
    }
    if (inst == 1) {
        fd = open(fifo_0, O_RDONLY);
    }
    if (fd == -1)
        printf("Open fifo error!\n");
    char recv[256];
    while (flag == 1){
        read(fd, recv, 255);
        memset(recv + 255, '\0', 1);
        printf("Received: %s\n", recv);
        fflush(stdout);
        // exit
        if (strncmp(exit_str, recv, 3) == 0) {
            flag = 0;
            close(fd);
            pthread_exit(NULL);
        }
    }
}


int main(int argc, char **argv){

// must enter the instance number
    if(argc!=2){
        printf("Usage: ./final [0|1]\n");
        return -1;
    }
    inst = atoi(argv[1]);
    if(inst != 0 && inst !=1){
        printf("Usage: ./final [0|1]\n");
        printf(" instance must be 0 or 1\n");
        return -1;
    }
    mkfifo(fifo_0, 0666);
    mkfifo(fifo_1, 0666);


    pthread_t tid1, tid2;
    pthread_attr_t attr;
    pthread_attr_init(&attr);

    pthread_create(&tid1, &attr, send_thread, NULL);
    pthread_create(&tid2, &attr, recv_thread, NULL);

    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);

    pthread_cancel(tid1);
    pthread_cancel(tid2);
    return 0;
}
