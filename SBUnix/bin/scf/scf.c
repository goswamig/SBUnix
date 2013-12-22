#include <stdio.h>

int main(int argc, char* argv[]) {
     printf("Enter a number and string\n"); 
     setforeground();
     int id;
     id = fork();
     if(id ==0) while(1);
     else
     { 
       int j;
       char s[100];
      scanf("%d",&j);
       printf("%d",j);   
       scanf("%s",s);
       printf("\n%s",s); 
      while(1); 
     }
      while(1);   
      return 0;
}
