#include <defs.h>
#include <system.h>
#include <syscall.h>
int  sleep(int time)
{
  return (int)(__syscall1(SLEEP, time));
}  
