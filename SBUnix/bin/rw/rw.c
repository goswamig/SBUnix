#include <stdio.h>

int main(int argc, char* argv[]) {
       printf("Describing read() and write() system call\n");  
       printf("Please type something !!!\n");  
       setforeground();  
       int id; 
       id = fork();
       if(id == 0) while(1);   //creating a dummy child so that when parent yields child can go to schedule  
       else
       { 
        char buff[128];
        int j; 
        j =read(0,buff,128);
        printf("Read %d characters\n",j);
        j = write(1,buff,j);
        printf("\nWrote %d characters\n",j);   
       }
       while(1); 
       return 0;
}
