/* Copyright © 2013, by Gautam kumar, Haseeb Niaz, Krishna Kanhaiya */
#include <defs.h>
#include <system.h>
#include <syscall.h>
#define COLUMNS 80 

int shift_key = 0;
int capslock_key = 0;
unsigned char kbdus[256] =
{
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8',	/* 9 */
  '9', '0', '-', '=', '\b',	/* Backspace */
  '\t',			/* Tab */
  'q', 'w', 'e', 'r',	/* 19 */
  't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',		/* Enter key */
    0,			/* 29   - Control */
  'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',	/* 39 */
 '\'', '`',   0,		/* Left shift */
 '\\', 'z', 'x', 'c', 'v', 'b', 'n',			/* 49 */
  'm', ',', '.', '/',   0,					/* Right shift */
  '*',
    0,	/* Alt */
  ' ',	/* Space bar */
    0,	/* Caps lock */
    0,	/* 59 - F1 key ... > */
    0,   0,   0,   0,   0,   0,   0,   0,
    0,	/* < ... F10 */
    0,	/* 69 - Num lock*/
    0,	/* Scroll Lock */
    0,	/* Home key */
    0,	/* Up Arrow */
    0,	/* Page Up */
  '-',
    0,	/* Left Arrow */
    0,
    0,	/* Right Arrow */
  '+',
    0,	/* 79 - End key*/
    0,	/* Down Arrow */
    0,	/* Page Down */
    0,	/* Insert Key */
    0,	/* Delete Key */
    0,   0,   0,
    0,	/* F11 Key */
    0,	/* F12 Key */
    0,	/* All other keys are undefined */
    0,  /*90*/
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 105*/
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 120*/
    0,0,0,0,0,0,0,      /*128*/
/*   CAPSLOCK KEY */  
   0,  27, '!', '@', '#', '$', '%', '^', '&', '*',     /* 9 */
  '(', ')', '_', '+', '\b',     /* Backspace */
  '\t',                 /* Tab */
  'Q', 'W', 'E', 'R',   /* 19 */
  'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n', /* Enter key */
    0,                  /* 29   - Control */
  'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':',     /* 39 */
 '\"', '~',   0,                /* Left shift */
 '\\', 'Z', 'X', 'C', 'V', 'B', 'N',                    /* 49 */
  'M', '<', '>', '?',   0,                              /* Right shift */
  '*',
    0,  /* Alt */
  ' ',  /* Space bar */
    0,  /* Caps lock */
    0,  /* 59 - F1 key ... > */
    0,   0,   0,   0,   0,   0,   0,   0,
    0,  /* < ... F10 */
    0,  /* 69 - Num lock*/
    0,  /* Scroll Lock */
    0,  /* Home key */
    0,  /* Up Arrow */
    0,  /* Page Up */
  '-',
    0,  /* Left Arrow */
    0,
    0,  /* Right Arrow */
  '+',
    0,  /* 79 - End key*/
    0,  /* Down Arrow */
    0,  /* Page Down */
    0,  /* Insert Key */
    0,  /* Delete Key */
    0,   0,   0,
    0,  /* F11 Key */
    0,  /* F12 Key */
    0,  /* All other keys are undefined */
};


//static int timer_ticks = 0;
static char *display = (char *)VIDEO_VIRTUAL_MEMORY;

void x86_64_handle_isr_vector0(struct regs *r)
{
	printf("Recieved interupt 0\n");
	__asm__( "hlt");
}


void x86_64_handle_isr_vector1(struct regs *r)
{
	//printf("SHEBANG\n");
	//__asm__( "hlt");
 
 //printf("Hello World\n");
 
 static int timer_ticks = 0;

 static int h1=0,h2=0,m1=0,m2=0,s1=0,s2=0,sec=0,min=0,hrs=0;
  int i,j; 
  i = 78;
  j = 49;
 timer_ticks++;
  //if(timer_ticks%10==0) 
   //printf("Ticking...%d\n",timer_ticks); 

//printf("Register r 0x%p\n",r);
// printf("Register Number %d\n",r->int_no); 
   
 if(timer_ticks % 18 == 0)
  {
 // 	printf("%d passed", timer_ticks);	
        sec++; 
        if(sec % 60 == 0)
         {
           min++;
           sec=0; 
         }  
        if(min % 60 == 0 && min>0)
         {
           hrs++;
           min=0;
         }
        if(hrs==24)
          hrs=0;

       s1 = sec % 10;
       s2 = sec / 10;
       m1 = min % 10;
       m2 = min / 10;
       h1 = hrs % 10;
       h2 = hrs / 10; 
  *(display + j*COLUMNS + i) = (char)((int)'0' + s1);
  i = i - 2;
  *(display + j*COLUMNS + i) = (char)((int)'0' + s2);
  i = i - 2;
  *(display + j*COLUMNS + i) = ':';
  i = i - 2;
  *(display + j*COLUMNS + i) = (char)((int)'0' + m1);
  i = i - 2;
  *(display + j*COLUMNS + i) = (char)((int)'0' + m2);
  i = i - 2;
  *(display + j*COLUMNS + i) = ':';
  i = i - 2;
  *(display + j*COLUMNS + i) = (char)((int)'0' + h1);
  i = i - 2;
  *(display + j*COLUMNS + i) = (char)((int)'0' + h2);
  i = i - 2;  
}

  //if (r->int_no >= 40)
    outb(0xA0,0x20);
 
  outb(0x20, 0x20);
}

/*
void x86_64_handle_isr_vector1(struct regs *r)
{
   printf("Hello Timer\n");
}
*/
void x86_64_handle_isr_vector2(struct regs *r)
{
 
    unsigned char scancode;

    /* Read from the keyboard's data buffer */
    scancode = inb(0x60);

    /* If the top bit of the byte we read from the keyboard is
    *  set, that means that a key has just been released */
    if (scancode & 0x80)
    {
        /* You can use this one to see if the user released the
        *  shift, alt, or control keys...
        *  42 stand for left shift and 54 for right shift */ 
       if((scancode & 0x7F)==42 || (scancode & 0x7F)==54)
            shift_key = 0;
    }
    else
    {
        /* Here, a key was just pressed. Please note that if you
        *  hold a key down, you will get repeated key press
        *  interrupts. */

        /* Just to show you how this works, we simply translate
        *  the keyboard scancode into an ASCII value, and then
        *  display it to the screen. You can get creative and
        *  use some flags to see if a shift is pressed and use a
        *  different layout, or you can add another 128 entries
        *  to the above layout to correspond to 'shift' being
        *  held. If shift is held using the larger lookup table,
        *  you would add 128 to the scancode when you look for it */
       
        if (scancode==42 || scancode ==54)
          shift_key = 1;
        else if(scancode == 58 || scancode==186)
          capslock_key = !capslock_key;  
        else
        {
         if(shift_key||capslock_key) 
         { 
           *(display + 49*COLUMNS + 60) = kbdus[scancode+128]; 
          struct task_struct *pcb = get_process(foreground_process);
          if(pcb) 
           pcb->buf[pcb->cursor++] = kbdus[scancode + 128]; 

           //putchar(kbdus[scancode]);
         } 
         else
         { 
           struct task_struct *pcb = get_process(foreground_process);
           if(pcb) 
            pcb->buf[pcb->cursor++] = kbdus[scancode];  
           *(display + 49*COLUMNS + 60) = kbdus[scancode]; 
         } 
        }
    }
  
//  if (r->int_no >= 40)
    outb(0xA0,0x20);
    
   outb(0x20, 0x20);
}


void x86_64_handle_isr_vector3(struct regs *r)
{
uint64_t n,arg1,val=-1,arg2,arg3;
 __asm volatile("movq %%rax, %0;"
                "movq %%rsi, %1;"
                "movq %%rdi, %2;"
                "movq %%rdx, %3;"
                : "=g"(n),"=g"(arg1),"=g"(arg2),"=g"(arg3)
                :
                :"rax","rsi","rdi","rdx"
 );


  if(n == PUTCHAR)   // putchar 1
  {
    putch((char)arg1);// call kernel putchar   
    asm volatile("movq %0, %%rax":: "b"(val)); //store return valueo;
  } 
  else if(n == EXIT)  
  {
    // exit status has been called with value in %rsi 
    exit_process(arg1);    
    asm volatile("movq %0, %%rax":: "b"(val)); //store return valueo;
  }
  else if(n == FORK)
  {
    val = fork();  
    asm volatile("movq %0, %%rax":: "b"(val)); //store return valueo;
  }
  else if(n == GETPID)
  {
    val = get_pid(); 
    asm volatile("movq %0, %%rax":: "b"(val)); //store return valueo;
  }
  else if(n == GETPPID)
  {
    val = get_ppid();
    asm volatile("movq %0, %%rax":: "b"(val));  // store return valueo; 
  } 
  else if(n == MALLOC)
  {              // j is size  
    void *val = malloc(arg1);   // allocate the heap memory with size j 
  //  printf("MALLOC %p\n",val);   
    asm volatile("movq %0, %%rax":: "b"(val)); //store return valueo;
  }
  else if(n == SCANF)
  {

    asm volatile("movq %0, %%rax":: "b"(val)); //store return valueo;
  }
  else if(n == SLEEP)
  {
    val = sleep(arg1);   
    asm volatile("movq %0, %%rax":: "b"(val)); //store return valueo;
  }
  else if(n == EXECV)
  {
    val = execvp(arg1,arg2,arg3);   
    asm volatile("movq %0, %%rax":: "b"(val)); //store return valueo;
  }
  else if(n == WAITPID)
  {
    val = waitpid(arg1,arg2); 
    asm volatile("movq %0, %%rax":: "b"(val)); //store return valueo;
  }
  else if(n == READ_STDIN)  //std input for process read    
  {
     val = read_process(arg1,arg2,arg3);  
     asm volatile("movq %0, %%rax":: "b"(val)); //store return valueo;
  }
  else if(n == WRITE_STDOUT)
  {
     val = write_process(arg1,arg2,arg3);  
     asm volatile("movq %0, %%rax":: "b"(val)); //store return valueo;
  }
  else if(n == WRITE_STDERR)
  {
  
     asm volatile("movq %0, %%rax":: "b"(val)); //store return valueo;
  } 
  else if(n == OPENDIR)
  {
     val = open_dir((char*)arg1);
     asm volatile("movq %0, %%rax":: "b"(val)); //store return valueo;
  }
  else if(n == CLOSEDIR)
  {
     val = closedir((char*)arg1);
     asm volatile("movq %0, %%rax":: "b"(val)); //store return valueo;
  }
  else if(n == READDIR) // list out all the directory name   
  {
     val = read_dir(arg1);
     asm volatile("movq %0, %%rax":: "b"(val)); //store return valueo;
  }
  else if(n == FOREGROUND)
  {
      setforeground();
     asm volatile("movq %0, %%rax":: "b"(val)); //store return valueo;
  }   
  else if(n == PS)
  {
     dp();
     asm volatile("movq %0, %%rax":: "b"(val)); //store return valueo;
  }
  else if(n == OPEN)
  {
     val = open((char*)arg1);  //open a file  
     asm volatile("movq %0, %%rax":: "b"(val)); //store return valueo;
  }
  else if(n == READ_STDF)   // read the file content  
  {
     val = read_file(arg1,arg2,arg3);
     asm volatile("movq %0, %%rax":: "b"(val)); //store return valueo;
  }
}



void x86_64_handle_isr_vector13(struct regs *r) //gpf handler  
{
  printf("General Protection Fault pid %d addr %p !!!\n",get_pid(),r);
  asm volatile("hlt"); 
}


/*********************** GOOD BYE BOSS i WILL HARDLY CALL YOU LET'S SEE ********************/  
void x86_64_handle_isr_vector14(struct regs *r) //pf handler 
{

  
  uint64_t addr;
//  printf("Page Fault %p!!!\n",r);
//  asm volatile("hlt"); 
  asm volatile ( "movq %%cr2, %0;" :"=g"(addr) );  
//  printf("addres %p\n", addr);  
  // first find the vma  
  struct task_struct *pcb = get_current_process();
  struct vm_area_struct * vm= pcb->mm->mmap;
  struct vm_area_struct *prev_vma=NULL; 
  struct vm_area_struct *curr_vma = pcb->mm->mmap; 
//  printf("start %p end %p size %d\n",vm->vm_start,vm->vm_end,vm->vm_mmsz);
  while(vm != NULL)
   {
    if(vm->vm_start <= addr && vm->vm_end > addr)
      break;
     vm = vm->vm_next;
   }
  while(curr_vma != NULL)
  {
 //   printf("VMA start %p, end %p\n",curr_vma->vm_start,curr_vma->vm_end);    
    prev_vma = curr_vma;
    curr_vma = curr_vma->vm_next; 
  }
  uint64_t s,e; //heap memory starting and end at max 1 page;
  s = ALIGN_DOWN((uint64_t)(prev_vma->vm_end + 0x1000));
  e = s + 0x1000;
//  printf("s %p e %p  addr %p heap start %p end %p \n",s,e,addr,pcb->heap_vma->vm_start,pcb->heap_vma->vm_end); 

 if(vm == NULL&& (addr<s||addr>e)) //neither vma associated with this process nor heap memory 
   {
     printf("\nSegmentation Fault process id %d pid %d at addr 0x%p  !!!\n",pcb->pid,get_pid(),addr);
     asm volatile("hlt"); 
     exit_process(-1); // exit current procesS
   }
 //  printf("start %p end %p size %d\n",vm->vm_start,vm->vm_end,vm->vm_mmsz);
 
if(vm != NULL) {
 if(vm->vm_file!= NULL)  // VMA IS ASSOCIATED WITH FILe 
 {
    // printf("File loading\n");
     uint64_t* pte = Pte((uint64_t *)vm->vm_start);
     if((*pte & PRESENT) == 0) // if frame is not allocated in pm
     {
       uint64_t* frame = pmmngr_alloc_block();
       if(frame == 0)
        {
          printf("I can't allocate here good bye\n");
          exit_process(-1);   // frame allocation failed
       }
        *pte =(uint64_t)frame | PRESENT | WRITE | USER;
     }
     int j;
     for(j=0; j<vm->vm_mmsz; j++)
     {
      *((uint64_t *)(vm->vm_start + j)) = *((uint64_t *)((uint64_t)vm->vm_file + vm->vm_offset + j));
//    printf("%c %c\n",*((uint64_t *)(vm->vm_start + j)),*((uint64_t *)((uint64_t)vm->vm_file + vm->vm_offset + j)));  
     } 
    
 //  printf("Loading complete\n"); 
//   asm volatile("hlt");  
}
 }
 else if((addr>=s)&&(addr<=e)) //this is unknown vma corresponding to malloc 
 {
//  printf("Heap Allocation !!!\n");
//  asm volatile("hlt");  
  uint64_t* pte = Pte((uint64_t *)pcb->heap_vma->vm_start);   // this is heap allocation  4kb max malloc_allocation 
     if((*pte & PRESENT) == 0) // if frame is not allocated in pm
     {
       uint64_t* frame = pmmngr_alloc_block();
       if(frame == 0)
        {
          printf("I can't allocate here good bye\n");
          exit_process(-1);   // frame allocation failed
       }
        *pte =(uint64_t)frame | PRESENT | WRITE | USER;
     }
//    printf("Heap Loaded !!!\n");  
   // asm volatile("hlt"); 
}
 else
  {
     printf("SEGMENTATION FAULT ....addr %d!!!\n",addr);
     asm volatile("hlt");  /***************************** UN COMMENT THIS LINE ******************************/ 
     exit_process(-1);  
 }
 // printf("Successfully handled a page fault\n");  
 // printf("HELLLLLLoooooooo\n"); 
//   asm volatile("hlt");   
}

void x86_64_handle_isr_vector4(struct regs *r)
{
    //    printf("Recieved interupt 0\n");
    //    __asm__( "hlt");
}


 
