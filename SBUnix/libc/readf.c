#include <defs.h>
#include <system.h>
#include <syscall.h>

int readf(uint64_t fd, int size,uint64_t buf) {
 return  __syscall3(READ_STDF,fd,size,buf);
}
