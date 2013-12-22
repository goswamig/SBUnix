#include <defs.h>
#include <system.h>
#include <syscall.h>
uint64_t readdir(uint64_t name)
{
  return __syscall1(READDIR,(uint64_t)name);
}  
