#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/mman.h>
#include <errno.h>
//#include <wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#define MAXQUEUE 10
#define PORT 6666
// generate fib number
int fib()
{
    static int i = 0, prev = 1, next = 1;
    i++;
    if (i == 1 || i == 2)
        return 1;
    int prev1 = prev;
    int next1 = next;
    prev = next;
    next = prev1 + next1;
    return prev1 + next1;
}

int main(int argc, char **argv)
{
    if (argc <= 1)
        return 1;
    int n = atoi(*(argv + 1));
    srand( (unsigned)time(NULL));
    pid_t fork_id = fork();
    // child producer process
    if (fork_id == 0)
    {
        int listenfd, connfd;
        struct sockaddr_in servaddr;

        // listenfd
        if((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1 ){
            printf("create socket error\n");
            return 0;
        }

        // address can rebind
        int option = 1;
        setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));

        // set servaddr
        memset(&servaddr, 0, sizeof(servaddr));
        servaddr.sin_family = AF_INET;
//        servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
        servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
        servaddr.sin_port = htons(PORT);

        if(bind(listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) != 0){
            fprintf(stderr, "socket: %s , errno %d\n", strerror(errno), errno);
            close(listenfd);
            exit(-1);
        }
        sleep(rand() % 5 + 1);
        if(listen(listenfd, MAXQUEUE) == -1){
            printf("listen socket error\n");
            exit(-1);
        }
//        printf("======waiting for client's request======\n");
        sleep(rand() % 5 + 1);
        if((connfd = accept(listenfd, (struct sockaddr*)NULL, NULL)) == -1) {
            printf("accept socket error\n");
        }
        for (int i = 0; i < n; i++) {
            int result = fib();
            sleep(rand() % 2);
            write(connfd, &result, sizeof(result));
        }
        close(connfd);
        close(listenfd);
        return 0;
    }
    // parent (consumer) as the client
    else if (fork_id > 0)
    {
        int sockfd = 0;

        struct sockaddr_in serv_addr;
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(PORT);
        serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

        int conn_result = -1;
        while(conn_result < 0)
        {
            if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
            {
                printf("Socket creation error \n");
                return -1;
            }
            conn_result = connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
            if (conn_result < 0) {
                close(sockfd);
                usleep(100000);
            }
        }
        for (int i = 0; i < n; i++) {
            int result;
            read(sockfd,&result,sizeof(result));
            printf("%d\n",result);
        }
        close(sockfd);
        int status;
        wait(&status);
        return 0;
    } else {
        printf("fork failed");
    }
    return 0;
}
