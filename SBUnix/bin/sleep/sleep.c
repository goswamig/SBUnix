#include <stdio.h>

int main(int argc, char* argv[]) {
        printf("parent: pid:%d Describing sleep()\n",getpid()); 
        int pid; 
        pid=fork();
        if(pid == 0)
        {
               printf("child: pid:%d ppid:%d sleeping() for 6 sec...\n",getpid(),getppid());  
               sleep(100);  //100/18 sec  
               printf("child: pid:%d ppid:%d waking up()...\n",getpid(),getppid());   
        } 
        else
        {
               printf("parent: pid:%d ppid:%d cpid:%d\n",getpid(),getppid(),pid);
        }    
        printf("bye from pid:%d\n",getpid());
        while(1);
	return 0;
}
