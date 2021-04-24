#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <time.h>



int fib(void) {
	int *ptr;
	int SIZE = 5 * sizeof(int);
	int shm_fd = shm_open("shm", O_RDWR, 0666);
    	ptr = mmap(0, SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, shm_fd, 0);        
	int prev1 = ptr[0];
	int prev2 = ptr[1];
	int next = prev1 + prev2;
	ptr[0] = prev2;
	ptr[1] = next;
	return next;	
}




int main(int argc, char** argv) {
    	// get "n" from command line
    	int n = atoi(argv[1]);	
		
	// create shared-mem "shm"
    	const char *name = "shm";
    	const int SIZE = 5 * sizeof(int);
    	int shm_fd;
    	int *ptr;
    	shm_fd = shm_open(name, O_CREAT | O_RDWR, 0666);
    	ftruncate(shm_fd, SIZE);
    	ptr = mmap(0, SIZE, PROT_WRITE, MAP_SHARED, shm_fd, 0);
	ptr[0] = 1;
	ptr[1] = 1;
	ptr[2] = 1;
	
	
	int pid = fork();
	if (pid == 0) {
		int result;
		srand( (unsigned)time( NULL ) );
		for (int i = 0; i < n; i++) {
			sleep(rand() % 2);
			result = fib();
			ptr[2] = result;
		}
	} else if (pid > 0) {
		int print_times = 1;
		int tmp = 1;
		// print n times
		while (print_times <= n) {
			if (print_times <= 2) {
				printf("%d: %d\n", print_times, 1);
				print_times++;
			}
			if (print_times >2 && tmp != ptr[2]) {
				printf("%d: %d\n", print_times, ptr[2]);
				tmp = ptr[2];
				print_times++;
			}	
		}
		wait(NULL);
		shm_unlink(name);
	}                
}
