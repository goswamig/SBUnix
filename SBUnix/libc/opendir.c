#include <defs.h>
#include <system.h>
#include <syscall.h>
uint64_t opendir(char* name)
{
  return __syscall1(OPENDIR,(uint64_t)name);
}  
