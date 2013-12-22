#ifndef _SYSCALL_H
#define _SYSCALL_H

#include <defs.h>

#define SYSCALL_PROTO(n) static __inline uint64_t __syscall##n

/************* SYSTEM CALL ****************/

#define PUTCHAR         1    // Done 
#define GETPID          2    // Done 
#define FORK            3    // DONE 
#define MALLOC          4    // Done 
#define EXIT            5    // Done  
#define SCANF           6    //      
#define GETPPID         7    // Done 
#define SLEEP           8    // DOne         
#define EXECV           9    // DONE 
#define WAITPID        10    // DONE    
#define READ_STDIN     11    // DONE  
#define WRITE_STDOUT   12    // DONE 
#define WRITE_STDERR   13    // 
#define OPENDIR        14
#define CLOSEDIR       15
#define READDIR        16
#define FOREGROUND     17    //DONE   
#define PS             18
#define OPEN           19    //done  
#define READ_STDF      20 


SYSCALL_PROTO(0)(uint64_t n) {
        uint64_t val;
    __asm volatile("movq %1,%%rax;"
                   "int $0x80;"
                   "movq %%rax,%0;"
                   :"=r"(val)
                   :"r"(n)
                   :"rax"//Clobberred registers
                   );
    return val;
}


SYSCALL_PROTO(1)(uint64_t n, uint64_t a1) {
        uint64_t val;
    __asm volatile("movq %1,%%rax;"
                   "movq %2,%%rsi;"
                   "int $0x80;"
                   "movq %%rax,%0;"
                   :"=r"(val)
                   :"r"(n),"r"(a1)
                   :"rax","rsi"//Clobberred registers
                   );
   return val;
}

SYSCALL_PROTO(2)(uint64_t n, uint64_t a1, uint64_t a2) {
    uint64_t val;
    __asm volatile("movq %1,%%rax;"
                   "movq %2,%%rsi;"
                   "movq %3,%%rdi;"
                   "int $0x80;"
                   "movq %%rax,%0;"
                   :"=r"(val)
                   :"r"(n),"r"(a1),"r"(a2)
                   :"rax","rsi","rdi"//Clobberred registers
                   );
    return val;
}

SYSCALL_PROTO(3)(uint64_t n, uint64_t a1, uint64_t a2, uint64_t a3) {
         uint64_t val;
    __asm volatile("movq %1,%%rax;"
                   "movq %2,%%rsi;"
                   "movq %3,%%rdi;"
                   "movq %4,%%rdx;"
                   "int $0x80;"
                   "movq %%rax,%0;"
                   :"=r"(val)
                   :"r"(n),"r"(a1),"r"(a2),"r"(a3)
                   :"rax","rsi","rdi","rdx"//Clobberred registers
                   );
    return val;
}

SYSCALL_PROTO(4)(uint64_t n, uint64_t a1, uint64_t a2, uint64_t a3, uint64_t a4) {
        return 0;
}




/*
SYSCALL_PROTO(0)(uint64_t n) {
   uint64_t val;
      __asm volatile("movq %1,%%rax;"
                   "int $0x80;"
                   "movq %%rax,%0;"
                   :"=r"(val)
                   :"r"(n)
                   :"rax"//Clobberred registers
                   ); 
   return val;
}

SYSCALL_PROTO(1)(uint64_t n, uint64_t a1) {
   uint64_t val;
    __asm volatile("movq %1,%%rax;"
                   "movq %2,%%rsi;"
                   "int $0x80;"
                   "movq %%rax,%0;"
                   :"=r"(val)
                   :"r"(n),"r"(a1)
                   :"rax","rsi"//Clobberred registers
                   );   
   return val;  
}

SYSCALL_PROTO(2)(uint64_t n, uint64_t a1, uint64_t a2) {
    uint64_t val;
    __asm volatile("movq %1,%%rax;"
                   "movq %2,%%rsi;"
                   "movq %3,%%rdi;"
                   "int $0x80;"
                   "movq %%rax,%0;"
                   :"=r"(val)
                   :"r"(n),"r"(a1),"r"(a2)
                   :"rax","rsi","rdi"//Clobberred registers
                   );
    return val;
}

SYSCALL_PROTO(3)(uint64_t n, uint64_t a1, uint64_t a2, uint64_t a3) {
    uint64_t val;
    __asm volatile("movq %1,%%rax;"
                   "movq %2,%%rsi;"
                   "movq %3,%%rdi;"
                   "movq %4,%%rdx;"
                   "int $0x80;"
                   "movq %%rax,%0;"
                   :"=r"(val)
                   :"r"(n),"r"(a1),"r"(a2),"r"(a3)
                   :"rax","rsi","rdi","rdx"//Clobberred registers
                   );
    return val;
}

SYSCALL_PROTO(4)(uint64_t n, uint64_t a1, uint64_t a2, uint64_t a3, uint64_t a4) {
	return 0;
}
*/
#endif
