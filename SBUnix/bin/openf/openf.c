#include <stdio.h>

int main(int argc, char* argv[]) {
        uint64_t addr;   
	addr = open("bin/openf");
        printf("\nAddress of openf %p", addr);
        
	char buf[512];
        int size = readf(open("alpha"), 32, (uint64_t) buf);
        printf("\nSize of openf.c %d", size);


//  uint64_t addr = open("bin/etc");
//  printf("%p\n",addr);    
 // printf("hello\n");   
  while(1);    	
  return 0;
}
