#include <defs.h>
#include <system.h>
#include <syscall.h>

int write(int fd,void* buf, int size) {
 return  __syscall3(WRITE_STDOUT,fd,(uint64_t)buf,size);
}
