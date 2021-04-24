#include<stdio.h>
#include<unistd.h>
#include<string.h>
#include<sys/wait.h>


void fib(int n, int array[]) {
	// cal the fib
	int i = 2;
	while (i < n) {
		array[i] = array[i-1] + array[i-2];
		i++;
	}
	
	int pid =  fork();
	if (pid > 0) {
		wait(NULL);
		printf("parent exiting…\n");
	} 
	else if (pid == 0) {
		// fork print the buffer
		for (int j = 0; j < n; j++) {
			printf("%d\n", array[j]);
		}
		
		
	} else {
		printf("errors\n");
	}	
}




int main(int argc,char *argv[]) {
	// n should >= 2
	int n = 2;
	char *a = argv[1];
    	sscanf(a,"%d",&n);
    	if (n < 2) {
    		printf("warning : the array should larger than 2\n");
    		n = 2;
    	}
	int array[n];
	for (int i = 0; i < n; i++) {
		array[i] = 0;
	}
	array[0] = 1;
	array[1] = 1;
	fib(n, array);
}
