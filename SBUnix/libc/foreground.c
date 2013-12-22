#include <defs.h>
#include <system.h>
#include <syscall.h>
void setforeground()
{
   __syscall0(FOREGROUND); 
}  
