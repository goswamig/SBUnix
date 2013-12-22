#include <stdio.h>

int main(int argc, char* argv[]) {
while(1) printf("proc_3 pid:%d\n",getpid());
        char buff[128];
       int j; 
       j =read(0,buff,128);
       printf("number of characters read %d\n",j);
      
     
       j = write(1,buff,j);
       printf("\nnumber of characters write %d\n",j); 
       while(1); // { //read(0,buff,128);
        //           write(1,buff,128); } 
        while(1) printf("proc_3 pid:%d\n",getpid());   
/*
	printf("Process 3!\n");
        while(1) printf("proc3 pid: %d \n",getpid());

       // sleep(2);
       int i;
       for(i=0;i<20;i++) 
          printf("proc3  pid:%d\n",getpid());

       char fname[] ="bin/proc4";  
        execv(fname,0,0); 
        while(1) printf("process_pid: %d \n",getpid());
 */       
       // putch('G');   
	return 0;
}
