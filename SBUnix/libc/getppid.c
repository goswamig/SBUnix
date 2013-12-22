#include <defs.h>
#include <system.h>
#include <syscall.h>
int getppid()
{
  return __syscall0(GETPPID);
}  
