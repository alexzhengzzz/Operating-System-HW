#include <stdio.h>
#include <stdlib.h>
#include <time.h>
/*
 this is a simple hello world program
*/
int main() 
{     
      printf("Hello world! This s CS6233 Spring 2021\n");
      
      // generate random number
      srand((unsigned)time(NULL));
      int randomNumber = rand() % 99 + 0;
      printf("%d\n", randomNumber);
      
      // blank new line
      printf("\n");
      return 0;
}
