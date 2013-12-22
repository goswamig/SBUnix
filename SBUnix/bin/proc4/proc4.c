#include <stdio.h>

int main(int argc, char* argv[]) {
        while(1) printf("proc_4 pid:%d\n",getpid());   

  int id = fork();
  if(id==0) 
    {  
    // while(1) printf("I am child pid %d\n",getpid());
     int i;
     for(i=0;i<25;i++)
      printf("I am forked child pid %d\n",getpid());  
//     execv("bin/proc5",0,0);  
    } 
  else
   { 
     waitpid(id,0);  
     while(1) printf("I am parent pid %d\n",getpid()); 
   }

//	printf("Process 4!\n");
//        sleep(5);
//        while(1) printf("proc4 pid: %d\n",getpid());
       // putch('G');   
 	
      return 0;
}
