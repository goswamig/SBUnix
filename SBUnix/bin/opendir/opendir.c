#include <stdio.h>
#include<defs.h>

int main(int argc, char* argv[]) {
  uint64_t te = opendir("bin/");
  readdir(te); 
  while(1);    	
  return 0;
}
