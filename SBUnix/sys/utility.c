#include <defs.h>
#include <system.h>

void *memcpy(void *dest, const void *src, int count)
{
    const char *sp = (const char *)src;
    char *dp = (char *)dest;
    for(; count != 0; count--) *dp++ = *sp++;
    return dest;
}

unsigned short *memsetw(unsigned short *dest, unsigned short val, int count)
{
    unsigned short *temp = (unsigned short *)dest;
    for( ; count != 0; count--) *temp++ = val;
    return dest;
}

/* 0 means both strings are same and nonzero value means both strings are different */
int strcmp(const char *s1, const char *s2)
{
  while(*s1 && (*s1==*s2))
        s1++,s2++;
    return *(const unsigned char*)s1-*(const unsigned char*)s2;
}


int strlen(const char *str)
{
    int retval;
    for(retval = 0; *str != '\0'; str++) retval++;
    return retval;
}

void* memset(void *ptr, int c, int n)
{
  unsigned char *s=ptr;
  while(n--)
   *s++ = (unsigned char)c;

  return ptr;
}

/* Given a octal number convert into decimal number */

int oct2dec(int n)
{

  int res = 0;
  int i=1;
  while(n)
  {
    res = res + ((n%10)*i);
    i = i*8;
    n = n/10;
  }
  return res;
}

/*  convert a string to number */
int  atoi(char *str)
{
    int res = 0;
    int i;
    for (i = 0; str[i] != '\0'; ++i)
        res = res*10 + str[i] - '0';

    return res;
}

/* convert a number to string  */
void itoa(long int value, char *out, int radix)
{
    char temp[40];
    char *ptr = temp;
    long int i;
    unsigned long v;
    int sign;

    sign = (radix == 10 && value < 0);
    if (sign)
    {
      v = -value;
    }else{
      v = (unsigned)value;
    }
    while (v || ptr == temp)
    {
        i = v % radix;
        v /= radix;
        if (i < 10)
          *ptr++ = i+'0';
        else
          *ptr++ = i + 'A' - 10;
    }

    if (sign)
    {
      *out++ = '-';
    }
    while (ptr > temp)
    {
      *out++ = *--ptr;
    }
    *out++ = '\0';
}

int pow(int x, int y)
{
    int temp;
    if( y == 0)
        return 1;
    temp = pow(x, y/2);
    if (y%2 == 0)
        return temp*temp;
    else
        return x*temp*temp;
}

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

char* strcpy(char *dst, const char *src) {
	char *ret;

	ret = dst;
	while ((*dst++ = *src++) != '\0')
		/* do nothing */;
	return ret;
}
int strncmp(const char *p, const char *q, size_t n) {
	while (n > 0 && *p && *p == *q)
		n--, p++, q++;
	if (n == 0)
		return 0;
	else
		return (int) ((unsigned char) *p - (unsigned char) *q);
}

