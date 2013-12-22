#include <defs.h>
#include <system.h>
#include <syscall.h>
int  execv(char *g1,char **g2,char **g3)
{
  return (int)(__syscall3(EXECV,(uint64_t)g1,(uint64_t)g2,(uint64_t)g3));
}  
