#include <defs.h>
#include <syscall.h>
#include<stdio.h>


uint64_t stoi(char *s) // the message and then the line #
{
    uint64_t i;
    i = 0;
    while(*s >= '0' && *s <= '9')
    {
        i = i * 10 + (*s - '0');
        s++;
    }
    return i;
}
int strcmp2(const char *p, const char *q) {
	while (*p || *q) {
		if (*p != *q)
			return -1;
		p++, q++;
	}
	return 0;
}

void scanf(char* str, void* buf) {
  //printf("hello\n");  
   char temp_buf[20]={'\0'};
   read(0,temp_buf,20);
 
    if(strcmp2(str, "%s") == 0)
    {
        buf = (char *)buf;
        int i = 0;
        while(temp_buf[i] != '\0')
            *(char *)buf++ = temp_buf[i++];
        *(char *)buf = '\0';
    }
    else if(strcmp2(str, "%d") == 0)
    {
        *(int *)buf = stoi(temp_buf);
    }
    else if(strcmp2(str, "%x") == 0)
    {
    }

}












