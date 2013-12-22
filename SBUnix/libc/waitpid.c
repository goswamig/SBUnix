#include <defs.h>
#include <system.h>
#include <syscall.h>

int waitpid(int pid,int status) {
 return  __syscall2(WAITPID,pid, status);
}
