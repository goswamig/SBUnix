#include <stdio.h>

int main(int argc, char* argv[]) {
  printf("welcome to test you have called me using execv!!!\n");
  printf("test pid:%d ppid:%d\n",getpid(),getppid());   
  while(1);    	
  return 0;
}
