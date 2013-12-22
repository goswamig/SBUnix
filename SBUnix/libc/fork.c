#include <defs.h>
#include <system.h>
#include <syscall.h>
int fork()
{
  return __syscall0(FORK); 
}  
