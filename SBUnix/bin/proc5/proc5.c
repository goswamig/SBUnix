#include <stdio.h>

int main(int argc, char* argv[]) {
    // sleep(10); 
     int i;
     for(i=0;i<100;i++)
       printf("I am execv child pid %d\n",getpid());
     
 

//       while(1) printf("proc_5 pid:%d\n",getpid());   

/*  int id = fork();
  if(id==0)
    while(1) printf("I am child pid %d\n",getpid());
  else
   while(1) printf("I am parent pid %d\n",getpid()); 
*/
//	printf("Process 4!\n");
//        sleep(5);
//        while(1) printf("proc4 pid: %d\n",getpid());
       // putch('G');   
 	
      return 0;
}
