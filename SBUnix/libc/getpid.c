#include <defs.h>
#include <system.h>
#include <syscall.h>
int getpid()
{
  return __syscall0(GETPID);
}  
