#include <stdio.h>

int main(int argc, char* argv[]) {
        printf("Parent: Describing exit system call\n");
        int pid; 
        pid=fork();
        if(pid ==0)
         {
                 printf("child: pid:%d ppid:%d\n",getpid(),getppid());
         } 
         else
         { 
                printf("parent: pid:%d ppid:%d cpid:%d\n",getpid(),getppid(),pid);
                int pid1 = fork();
                if(pid1 ==0)
                {
                    printf("child: pid:%d ppid:%d\n",getpid(),getppid());
                }
               else
                {
                    printf("parent: pid:%d ppid:%d cpid1:%d cpid2:%d\n",getpid(),getppid(),pid,pid1);
                }    
         }   
         if(getpid() == 1) // This will be the last process runing on OS  
         {
           printf("I am parent, I will stay forever\n"); 
           while(1);
         } 
         else
         {
           printf("Bye from pid:%d ppid:%d\n",getpid(),getppid());
         }  
	return 0;
}
