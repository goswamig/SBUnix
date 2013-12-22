#include <stdio.h>

int main(int argc, char* argv[]) {

        printf("parent: Describing heap and stack memory's copy between parent and child\n"); 
        printf("parent: pid:%d ppid:%d, using fork and malloc...\n",getpid(),getppid());     

        int var=10;  //stack memory   

        int *ip =(int *)malloc(sizeof(int)*3);  //heap memory 
        int pid;
        ip[0] = 0;
        ip[1] = 1;
        ip[2] = 2; 
        printf("parent's : stack: var=%d, heap: ip[%d]=%d, ip[%d]=%d, ip[%d]=%d\n",var,0,ip[0],1,ip[1],2,ip[2]);   
        pid=fork();
        if(pid ==0)
         {  
            printf("Child's: stack: var=%d\n",var);
            var = 30; 
            printf("Child's: stack: var=%d\n",var);
            ip[0] = 1; ip[1] = 2; ip[2] = 3;   
            printf("child's : heap: ip[%d]=%d, ip[%d]=%d, ip[%d]=%d\n",0,ip[0],1,ip[1],2,ip[2]);    
         } 
         else
         {
            printf("parent's: stack: var=%d\n",var);
            var = 20; //updating into parents's copy
            printf("parent's: stack: var=%d\n",var);
            ip[0]= 4; ip[1] = 5; ip[2] = 6;     
            printf("parent's : heap: ip[%d]=%d, ip[%d]=%d, ip[%d]=%d\n",0,ip[0],1,ip[1],2,ip[2]);    
         }    
        printf("bye from pid:%d \n",getpid());
        while(1);  
	return 0;
}
