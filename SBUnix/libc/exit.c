#include <defs.h>
#include <system.h>
#include <syscall.h>

void exit(int status) {
  __syscall1(EXIT, status);
}
