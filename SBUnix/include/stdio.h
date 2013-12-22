#ifndef _STDIO_H
#define _STDIO_H
#include<defs.h>  
/************** LIST OF SYSTEM CALLS IMPLEMENTED **********/
int printf(const char *format, ...);
void scanf(char* str, void* buf);    


int putch(char c);
void* malloc(int i); 
int sleep(int i);
int fork();
int getpid();
int getppid();
int exit(int);
int execv(char *, char **, char **);  
int waitpid(int i, int j);
int read(int,void*,int);  
int write(int,void*,int);   
uint64_t opendir(char *name);
uint64_t closedir(char *name);   
uint64_t readdir(uint64_t ptr);    
uint64_t open(char* file); 
void setforeground();
void ps();
int  readf(uint64_t,int,uint64_t); 
#endif
