#include <stdio.h>

int main(int argc, char* argv[]) {
  printf("Parent: pid:%d Describing waitpid()\n",getpid());  
  int pid = fork();
  if(pid==0) 
    { 
      printf("Child: pid:%d ppid:%d exiting()...\n",getpid(),getppid());
      return 0;   
   } 
  else
   { 
      waitpid(-1,0); // you can pass -1 for any child 
      printf("Parent: pid:%d\n",getpid()); 
   }
   while(1);   // parent will remain here
}
