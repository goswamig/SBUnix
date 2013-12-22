#include <stdio.h>

int main(int argc, char* argv[]) {
        printf("parent: pid:%d Describing execv()\n",getpid()); 
        int pid; 
        pid=fork();
        if(pid ==0)
         {
              printf("child: pid:%d ppid:%d test will be loaded...!!!\n",getpid(),getppid());
              execv("bin/test",0,0);
              printf("child: I'm not any more here\n");    
         } 
         else
         {
              printf("parent: pid:%d ppid:%d cpid:%d\n",getpid(),getppid(),pid);
         }    
        printf("bye from pid:%d\n",getpid());
        while(1);
	return 0;
}
