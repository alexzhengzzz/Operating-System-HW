#include <unistd.h>
#include <errno.h>
#include <wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <arpa/inet.h>

#define PORT 9999

int Fibonacci() {
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

int main(int argc, char **argv) {
	if (argc <= 1)
		return 1;

	int n = atoi(*(argv + 1));
	int key = 1111;

	if (fork() == 0) {
		struct sockaddr_in my_addr;
		struct sockaddr_in client_addr;
		int sockfd;
		int clientfd;
		int sin_size;

		/* create server socket*/
		sockfd = socket(AF_INET, SOCK_STREAM, 0);
		my_addr.sin_family = AF_INET;
		my_addr.sin_port = htons(PORT);
		my_addr.sin_addr.s_addr = htonl(INADDR_ANY);
		bzero(&(my_addr.sin_zero), 8);

        /* address can rebind */
        int option = 1;
        setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));
        
        /* bind socket*/
        bind(sockfd, (struct sockaddr*) &my_addr, sizeof(struct sockaddr));

		/* wait 3 seconds*/
		usleep(3 * 1000000);

		/* start listen*/
		listen(sockfd, 1);

		clientfd = accept(sockfd, (struct sockaddr*) &client_addr, &sin_size);

		srand(time(NULL));
		for (int i = 0; i < n; i++) {
			int num;

			usleep(rand() % 1000000);
			num = Fibonacci();

			send(clientfd, &num, sizeof(int), 0);
		}

		close(clientfd);
		close(sockfd);
		exit(0);
	} else {
		struct sockaddr_in my_addr;
		int sockfd;

		sockfd = socket(AF_INET, SOCK_STREAM, 0);

		my_addr.sin_family = AF_INET;
		my_addr.sin_port = htons(PORT);
		my_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
		bzero(&(my_addr.sin_zero), 8);

		/* keep connecting*/
		while (connect(sockfd, (struct sockaddr*) &my_addr,
				sizeof(struct sockaddr)) == -1) {
			usleep(100 * 1000);
		}

		printf("values:\n");
		for (int i = 0; i < n; i++) {
			int v;
			recv(sockfd, &v, sizeof(int), 0);

			printf(" %d \n", v);
		}
		printf("\n");

		close(sockfd);

		int status;
		wait(&status);
	}
	return 0;
}


/* Q: Which of calls are blocking and which are not? Explain what that means:

1. Blocking: connect(),accept(),write(),read()
2. Non-blocking: socket(), bind(), listen(), close()

 Blocking means that the calling process becomes in a "blocked" state (i.e. not running and not ready). As such it is won't be scheduled by the scheduler till an event happens resulting in moving the process back into the ready state (and ready queue). That event is usually an I/O operation completion, for example the connect() may block till the reception of a packet from the server indicating the connection is accepted.
 
 Blocking is considered synchronous whereas Non-blocking is considered asynchronous.
 

/* Q: Is this form of direct communication or indirect communication:

Indirect communication


/* Q: What is the failure flag returned from connect() that indicates server is not ready?

The call returns -1 and errno is set to ECONNREFUSED and -1 which means it found that no one is listening on the remote address, therefore the server is not ready.


/*Q: How would you change the program to communicate between process in a different machine

If you want to communicate on different computers, you need to know the IP address and port number of the server and have the client connect to them.


