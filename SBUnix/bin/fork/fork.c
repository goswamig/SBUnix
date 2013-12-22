#include <stdio.h>

int main(int argc, char* argv[]) {
        printf("parent: pid:%d ppid:%d forking...\n",getpid(),getppid()); 
        int pid; 
        pid=fork();
        if(pid ==0)
         {
                 printf("child: pid:%d ppid:%d\n",getpid(),getppid());
         } 
         else{
                printf("parent: pid:%d ppid:%d cpid:%d\n",getpid(),getppid(),pid);
             }    
        printf("bye from pid:%d\n",getpid());
        while(1);
	return 0;
}
