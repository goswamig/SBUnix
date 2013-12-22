#include <stdio.h>

int main(int argc, char* argv[]) {
  printf("My pid:%d, my parent's pid:%d !!!\n",getpid(),getppid());
  while(1);    	
  return 0;
}
