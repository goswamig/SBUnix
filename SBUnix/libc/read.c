#include <defs.h>
#include <system.h>
#include <syscall.h>

int read(int fd,void* buf, int size) {
 return  __syscall3(READ_STDIN,fd,(uint64_t)buf,size);
}
