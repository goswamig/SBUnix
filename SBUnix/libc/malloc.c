#include <defs.h>
#include <system.h>
#include <syscall.h>
void* malloc(uint64_t size)
{
  return (void *)(__syscall1(MALLOC, size));
}  
