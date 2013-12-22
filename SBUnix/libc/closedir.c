#include <defs.h>
#include <system.h>
#include <syscall.h>
uint64_t closedir(char* name)
{
  return __syscall1(CLOSEDIR,(uint64_t)name);
}  
