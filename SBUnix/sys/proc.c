#include <system.h>
#include <defs.h>
#include <sys/gdt.h>
extern void x86_64_isr_vector1(); 
 struct task_struct *prev;
 struct task_struct *next; 
 static int j=0;
 static int c=0;
 static int first_switch = 1;
struct task_struct *dummy_prev;
uint64_t no_of_process;
uint64_t reg[14]; // set of registers  
uint64_t child_ret_addr;
uint64_t child_stack_addr; 
uint64_t child_pid; 
static uint64_t PID=1;

struct task_struct* malloc_pcb(void *fun_ptr)
{
// printf("hello %p world %p proc3 %p proc4 %p\n", search("bin/hello"),search("bin/world"),search("bin/proc3"),search("bin/proc4")); 
// asm volatile("hlt");   

 // PID is not just pid it also gives the number of process 
// static uint64_t PID=1; 
 struct task_struct * pcb = (struct task_struct *)kmalloc(sizeof(struct task_struct));

 //printf("pcb %p \n",pcb);    
 if(!pcb)   
  return 0;

 ready_queue[++process_count] = (uint64_t )pcb;
 
 pcb->mm = (struct mm_struct *)((char *)(pcb + 1));
 pcb->mm->count = 0;
 pcb->mm->mmap = NULL;
 
 //fill the entry of pcb
 pcb->pid =PID++; 
 pcb->ppid = 0; //I'm kernel's child  
 //no_of_process = PID; // number of process currenly ready
 pcb->zombie = 0; // this is not zombie   
 pcb->iswait = 0; //no waitpid wait 
 pcb->readwait = 0; //not waiting fro any read
 pcb->sleep_time = 0; // this is not sleeping 
 pcb->cursor = 0; // for reading   
 pcb->prev_cursor = 0; 
 memcpy(pcb->pname,fun_ptr,strlen(fun_ptr));  
// printf("%s\n",pcb->pname);
// asm volatile("hlt"); 


uint64_t *pml4e =  kmalloc(512);
//initialize it
int j;
for(j=0;j<512;j++)
  pml4e[j] = 0;
pml4e[511] = *(Pml4e((uint64_t*)0xFFFFFFFF80200000)); //point to pdpe of kerne
pml4e[500] = ALIGN_DOWN((uint64_t)(*Pte(pml4e))) | PRESENT | WRITE; //self reference
pcb->pml4e = ALIGN_DOWN((uint64_t)(*Pte(pml4e)));

//my process stack grows upwards   
 pcb->bmpPtr = (uint64_t *)BUMP_PTR_USER_SPACE;

/* we need to switch process address space in order to allocate its stack */

   // asm volatile( "cli");
    asm volatile("movq %0, %%cr3":: "b"(pcb->pml4e));
  //  asm volatile( "sti");

// printf("bump ptr %p\n",pcb->bmpPtr);   //currently stack is being allocated static 
 pcb->stack = (uint64_t *)vmmngr_bump_alloc_process(&pcb->bmpPtr,PAGE_SIZE);  //lets allocate two page  


// printf("bump ptr %p stack %p \n",pcb->bmpPtr,pcb->stack);
 pcb->kstack[511] = 0x23; //ss
 pcb->kstack[510] = (uint64_t)(&(pcb->stack[511])); //rsp 
// printf("%p %p %p...kstack\n",pcb->kstack[510],pcb->stack,&(pcb->stack[1]));
//  printf("%s\n",fun_ptr);  
//  asm volatile("hlt");  
 pcb->kstack[509] = 0x200286; //rflags  
 pcb->kstack[508] = 0x1b; //cs
 pcb->kstack[507] = 0;  //rip entry point scroll down to see the value  

 pcb->kstack[506] = 1; pcb->kstack[505] = 2;  pcb->kstack[504] = 3;  pcb->kstack[503] = 4;
 pcb->kstack[502] = 5; pcb->kstack[501] = 6;  pcb->kstack[500] = 7;  pcb->kstack[499] = 8;
 pcb->kstack[498] = 9; pcb->kstack[497] = 10; pcb->kstack[496] = 11; pcb->kstack[495] = 12;
 pcb->kstack[494] = 13;pcb->kstack[493] = 14; pcb->kstack[492] = 15;     
 pcb->kstack[491] = (uint64_t)(x86_64_isr_vector1 + 34);
 pcb->kstack[490] = 16; //for extra rbx
 pcb->rsp = &(pcb->kstack[490]);

/*
 pcb->stack[511]=0x10; //ss  
 pcb->stack[510]=(uint64_t)(&(pcb->stack[511])); //RSP  ;
 pcb->stack[509]= 0x200286; //rflags 
 pcb->stack[508] = 0x8; //CS  
 pcb->stack[507] = (uint64_t)fun_ptr; //RIP  
 pcb->stack[506] = 1; pcb->stack[505] = 2;  pcb->stack[504] = 3;  pcb->stack[503] = 4;
 pcb->stack[502] = 5; pcb->stack[501] = 6;  pcb->stack[500] = 7;  
 pcb->stack[499] = (uint64_t)(x86_64_isr_vector1 + 22);
 pcb->stack[498] = 12; //for extra rbx
 pcb->rsp = &(pcb->stack[498]);
*/
/****************************** ELF ********************************/
// printf("hello %p world %p proc3 %p proc4 %p\n", search("bin/hello"),search("bin/world"),search("bin/proc3"),search("bin/proc4"));
// asm volatile("hlt");


int i;
 struct posix_header_ustar *file;
 file = (struct posix_header_ustar *)search(fun_ptr);
// printf("%p file\n",file); 
// asm volatile("hlt");   
 file = file + 1; // point to the elf header
 struct ELF *elf =(struct ELF *)file;
 //now i have elf header lets play with it
//lets get the section header from elfheader by using the offset of elfheader
 struct ELF_ph * elfph = (struct ELF_ph *)((uint64_t)elf + elf->e_phoff);
// printf(" %d elfphnum\n",elf->e_phnum);
// asm volatile("hlt");     
 for(i=0;i<elf->e_phnum;i++) {
  if(elfph->p_type ==  PT_LOAD)
   {
  //    printf("We need to load this segment %d %d %d\n",elfph->p_vaddr,elfph->p_memsz,elfph->p_offset);
    if(elfph->p_memsz > 4096)
        {
           printf("size is greater than a page.... the memory manager needs improvement !!!\n");  
           asm volatile("hlt"); 
        }  
     uint64_t* pte = Pte((uint64_t *)elfph->p_vaddr);
     if((*pte & PRESENT) == 0) // if frame is not allocated in pm
     {
       uint64_t* frame = pmmngr_alloc_block();
       if(frame == 0)
        {
          printf("I can't allocate here good bye\n");
          return 0;   // frame allocation failed
       }
        *pte =(uint64_t)frame | PRESENT | WRITE | USER;   
     }
    int j;  
    for(j=0; j<elfph->p_memsz; j++)
     {
      *((uint64_t *)(elfph->p_vaddr + j)) = *((uint64_t *)((uint64_t)elf + elfph->p_offset + j));
     }
   struct vm_area_struct *vm;
    vm = malloc_vma(pcb->mm);
    vm->vm_start = elfph->p_vaddr;
    vm->vm_end = vm->vm_start + elfph->p_memsz;
    vm->vm_mmsz = elfph->p_memsz;
    vm->vm_next = NULL;
    vm->vm_file =(uint64_t)elf;
    vm->vm_flags = elfph->p_flags;
    vm->vm_offset = elfph->p_offset;   
//    printf("start %p end %p size %d\n",vm->vm_start,vm->vm_end,vm->vm_mmsz); 
 }
  elfph= elfph + 1;
 }

 pcb->heap_vma = (struct vm_area_struct *)kmalloc(1); // allocate a separate page for heap vma, however this is waste
 //printf("heap_vma %p %p %p %d\n",pcb->heap_vma,pcb->heap_vma->vm_start,pcb->heap_vma->vm_end,pcb->heap_vma->vm_mmsz);  
 //asm volatile("hlt");  
 struct vm_area_struct *tmp = pcb->mm->mmap;
 while(tmp->vm_next != NULL)  tmp = tmp->vm_next;  // go to the last vma
 pcb->heap_vma->vm_start = pcb->heap_vma->vm_end = ALIGN_DOWN((uint64_t)(tmp->vm_end + 0x1000));  // start from next page (keep the distance)  
 pcb->heap_vma->vm_mmsz = 0x1000;
 uint64_t* pte = Pte((uint64_t *)pcb->heap_vma->vm_start);
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

// printf("%p %p\n",pcb->heap_vma->vm_start,pcb->heap_vma->vm_end);  
 pcb->kstack[507] = (uint64_t)elf->e_entry; //RIP
// printf("pcb %p %p %p \n",pcb->heap_vma->vm_start, pcb->heap_vma->vm_end ,tmp->vm_end);
// printf("entry %p  stack %p\n", elf->e_entry,pcb->kstack[507]); 
 /* switch back to kernel address space before we return */
 // asm volatile( "cli");
    asm volatile("movq %0, %%cr3":: "b"(kernel_pml4e));
 // asm volatile( "sti");
 //asm volatile("hlt");   
 return pcb;
}

void fun1()
{
// printf("Hello\n");
// while(1);

 while(1) 
 {
   printf("Hello\n");
  // schedule();
 asm volatile("sti"); 
 }

}
void fun2()
{

 //printf("World\n");
 //while(1);

 while(1)
 {
   printf("World\n");  
   //schedule();
   asm volatile("sti"); 
 }
}

void init_context_switch() {

printf("init %p %p %p %p\n",ready_queue[0],ready_queue[1],((struct task_struct *)ready_queue[0])->rsp,
((struct task_struct *)ready_queue[1])->rsp);
 //save rsp in prev's stack
 __asm__(
            "movq %%rsp, %0;"
            :"=g"(dummy_prev)
        );

 //change cr3
  __asm__("cli");
  asm volatile("movq %0, %%cr3":: "b"(((struct task_struct *)ready_queue[1])->pml4e));

 //load next's stack in rsp
 __asm__(
                "movq %0, %%rsp;"  //load next's stack in rsp
                :
                :"r"(((struct task_struct *)ready_queue[1])->rsp)
        );
     /* __asm__( "popq %rdx");
        __asm__( "popq %rcx");
        __asm__( "popq %rbx");
        __asm__( "popq %rax");  */  
}

void schedule()
{
 c++;
 if(c==1) {
  /* walk through the ready_queue and decrease their quantum if any one is sleeping
     we will skip the schedule of process which is sleeping */    
  int i,k;
   struct task_struct *pcb; 
   struct task_struct *pcb1;  
  for(i=1;i<=process_count;i++) {    
    pcb = (struct task_struct *)ready_queue[i]; 
      if(pcb->sleep_time > 0)
        pcb->sleep_time--;    
    if(pcb->prev_cursor>=pcb->cursor)
     pcb->readwait = 0; 

  }
  for(i=1;i<=process_count;i++)
  {
    pcb = (struct task_struct *)ready_queue[i];
    if(pcb->iswait)  // find the pid  
    {
      for(k=1;k<=process_count;k++)   
      {
        pcb1 = (struct task_struct *)ready_queue[k];
        if((pcb1->pid==pcb->iswait) && (pcb1->ppid ==pcb->pid))
         break; 
      }
      if(k>process_count)   //there is no child in ready_queue
      pcb->iswait = 0;  
    } 
  }

 
 if(first_switch) {
       prev = (struct task_struct *)ready_queue[0];
       int k=1;
       while(k<=process_count)  // find the first ready process wHo is not sleeping and not in wait 
       {
        pcb =(struct task_struct *) ready_queue[k];
        if((pcb->sleep_time == 0) && (pcb->iswait==0) &&(pcb->readwait==0)) 
         break;
        k++;
       } 
       next = (struct task_struct *)ready_queue[k];   
       __asm__(
               "movq %%rsp, %0;"
               :"=g"(prev->rsp)
             );
       asm volatile("movq %0, %%cr3":: "b"(next->pml4e));
       __asm__(
               "movq %0, %%rsp;"
               :
               :"r"(next->rsp)
             );
       first_switch = 0;  
       j = k; 
       tss.rsp0 =(uint64_t)(&(next->kstack[511]));    
      }
 else {
     // don't switch with kernel j will always be greater than 0 here 
      prev = (struct task_struct*) ready_queue[j];
      j = (j + 1) % (process_count+1);
      if(j == 0 )   // we don't do switch with kernel now 
       j = 1; 
      for(i=0;i<=process_count;i++)    // find the first awaken process in the round robin queue 
       {
         pcb = (struct task_struct *)ready_queue[j];
         if((pcb->sleep_time == 0) &&(pcb->iswait ==0)&&(pcb->readwait==0))
          break;
         j = (j+1)%(process_count+1);
         if(j == 0)
            j=1;   
       } 
        
      next =(struct task_struct *)ready_queue[j];
      __asm__(
               "movq %%rsp, %0;"
               :"=g"(prev->rsp)
             );
      asm volatile("movq %0, %%cr3":: "b"(next->pml4e));
      __asm__(
               "movq %0, %%rsp;"
               :
               :"r"(next->rsp)
             );
      } 
   tss.rsp0 = (uint64_t)(&(next->kstack[511]));
   // printf("u am here\n"); 
     c = 0; 
  }
}

//Round robincheduler
/*
void schedule()
{
 c++;
 if(c == 18) { 
 prev = (struct task_struct*) ready_queue[j];
 j = (j + 1) % no_of_process;
 next =(struct task_struct *)ready_queue[j];
 switch_to(prev,next);   
 }
}
*/

void exit_process(int status)  
{
 
// Load kernel's rsp
       __asm__(
               "movq %0, %%rsp;"
               :
               :"r"(((struct task_struct *)ready_queue[0])->rsp)
             );
/* you are in process page table so if you have allocated any memoryfree it   
   we need to free all vma's related stuff we will implement free later so now there is memory leak :(  
*/


/* load kernel's cr3 register */  
  asm volatile("movq %0, %%cr3":: "b"(kernel_pml4e));

// now we need to free the pcb's allocations, process kstack, process pml4e but we are not doing this memory leak :(  
  next->zombie = 1;  // mark this process as zombie;  
  zombie_queue[next->pid] = status;   // so you can get the exit status by using process pid 
  int i;
  //if your parent is waiting for you mark its waiting list   
/*  for(i=1;i<=process_count;i++)
  {
   if(next->ppid == ((struct task_struct *)ready_queue[i])->pid)
   {
     if(((struct task_struct *)ready_queue[i])->iswait==next->pid)  // if my parent is waiting for me i will not let them wait
       ((struct task_struct *)ready_queue[i])->iswait = 0;      
   } 
  } 
*/
  // now update the ready_queue and process_count
  // we are sure that current running process is "next" 

    i = 1;     
   while(i <= process_count)  //find the pcb in ready_queue 
    {
      if(ready_queue[i] == (uint64_t)next) 
         break;
      i++; 
    }       
   if(i < process_count)  // update the ready_queue 
    {
      while((i+1)<= process_count)
       {
         ready_queue[i] =  ready_queue[i+1];   
         i++;
       }  
    }
   process_count = process_count - 1; 
   // now you are in kernel mode so set it accorodingly 
   first_switch = 1;

//  for(i=0;i<100;i++)
//   printf("I am kernel\n");
   
//   printf("%d %p %p %p\n",process_count, ready_queue[1],ready_queue[2],ready_queue[3]);   
//  asm volatile("hlt");  
//   while(1);
   asm volatile("pop %rbx");  // extra rbx pop 
   asm volatile("pop %rax");  // get the isr addr
   asm volatile("jmp *%rax"); 
} 

int get_pid()
{
   return next->pid; 
}

int get_ppid()
{
  return next->ppid;
}

struct task_struct *get_current_process() //get current running process  
{
 return next;
}


struct task_struct *get_process(uint64_t pid) // get the process with pid from ready_queue or return NULL 
{
 int i;
 for(i=1;i<=process_count;i++)
  {
    if(((struct task_struct*)ready_queue[i])->pid == pid)
      return (struct task_struct*)ready_queue[i];   
  }
 return NULL; //if you don't find one return NULL;
} 

void*  malloc(uint64_t  size)
{
 struct task_struct *pcb = get_current_process();
 uint64_t old = pcb->heap_vma->vm_end;
 pcb->heap_vma->vm_end = pcb->heap_vma->vm_end + size;   
// printf("Malloc %p\n",old);
 if(pcb->heap_vma->vm_end - pcb->heap_vma->vm_start > 0x1000)
  {
    printf("Can't allocate memory\n");
    return NULL;
  }
 return (void *)old;     
}

void fork_process()
{
 // printf("rip child %p %p",child_ret_addr,child_stack_addr); 
 // asm volatile("hlt");   
  struct task_struct *parent_pcb = get_current_process();
  struct task_struct *child_pcb = (struct task_struct *)kmalloc(sizeof(struct task_struct)); 
  if(!child_pcb)
    return ;

  ready_queue[++process_count] =(uint64_t)child_pcb; 
  child_pcb->mm = (struct mm_struct *)((char *)(child_pcb + 1));
  child_pcb->mm->count = 0;
  child_pcb->mm->mmap = NULL;
  
 //fill the entry of pcb
  child_pcb->pid =PID++;
  child_pid = child_pcb->pid;  
  child_pcb->ppid = parent_pcb->pid; //I'm parent's child
 //no_of_process = PID; // number of process currenly ready
  child_pcb->zombie = 0; // this is not zombie
  child_pcb->iswait = 0; //no waitpid wait 
  child_pcb->readwait = 0; //not waiting for anything 
  child_pcb->sleep_time = 0; // this is not sleeping
  child_pcb->cursor = 0;
  child_pcb->prev_cursor = 0;
  memcpy(child_pcb->pname,parent_pcb->pname,strlen(parent_pcb->pname)); 
  uint64_t *pml4e =  kmalloc(512);

//initialize it
  int j;
  for(j=0;j<512;j++)
   pml4e[j] = 0;
  pml4e[511] = *(Pml4e((uint64_t*)0xFFFFFFFF80200000)); //point to pdpe of kerne
  pml4e[500] = ALIGN_DOWN((uint64_t)(*Pte(pml4e))) | PRESENT | WRITE; //self reference
  child_pcb->pml4e = ALIGN_DOWN((uint64_t)(*Pte(pml4e)));
 
  //my process stack grows downwards
  child_pcb->bmpPtr = (uint64_t *)BUMP_PTR_USER_SPACE;

  // lets copy the content of parent stack
   int *tmp_stack=(int *)kmalloc(512);
   
  for(j=0;j<512;j++)    
  {
   tmp_stack[j] = parent_pcb->stack[j];
//   printf(" %p %p ",tmp_stack[j],parent_pcb->stack[j]);
  }  
//  asm volatile("hlt");  
  /* we need to switch child process address space in order to allocate its stack */

   // asm volatile( "cli");
    asm volatile("movq %0, %%cr3":: "b"(child_pcb->pml4e));
  //  asm volatile( "sti");

// printf("bump ptr %p\n",pcb->bmpPtr);   //currently stack is being allocated static
  child_pcb->stack = (uint64_t *)vmmngr_bump_alloc_process(&child_pcb->bmpPtr,PAGE_SIZE);  //lets allocate two page
// printf("%p ..child\n",&child_pcb->stack[511]); 
 //asm volatile("hlt"); 

 //copy the stack
 for(j=0;j<512;j++)
  child_pcb->stack[j] = tmp_stack[j]; 

 child_pcb->kstack[511] = 0x23; //ss
 child_pcb->kstack[510] = child_stack_addr; //rsp
 child_pcb->kstack[509] = 0x200286; //rflags
 child_pcb->kstack[508] = 0x1b; //cs
 child_pcb->kstack[507] = child_ret_addr;  //rip entry point scroll down to see the value

 child_pcb->kstack[506] = 0; //rax return to child 

 child_pcb->kstack[505] = reg[0];  child_pcb->kstack[504] = reg[1];  child_pcb->kstack[503] = reg[2];
 child_pcb->kstack[502] = reg[3]; child_pcb->kstack[501] = reg[4];  child_pcb->kstack[500] = reg[5];  child_pcb->kstack[499] = reg[6];
 child_pcb->kstack[498] = reg[7]; child_pcb->kstack[497] = reg[8]; child_pcb->kstack[496] = reg[9]; child_pcb->kstack[495] = reg[10];
 child_pcb->kstack[494] = reg[11];child_pcb->kstack[493] = reg[12];
 child_pcb->kstack[492] = reg[13];  
 child_pcb->kstack[491] = (uint64_t)(x86_64_isr_vector1 + 34);
 child_pcb->kstack[490] = 16; //for extra rbx
 child_pcb->rsp = &(child_pcb->kstack[490]);

 struct vm_area_struct* parent_vm = parent_pcb->mm->mmap;
 struct vm_area_struct* child_vm;
 while(parent_vm!=NULL) 
 { 
    child_vm = malloc_vma(child_pcb->mm);
    child_vm->vm_start = parent_vm->vm_start;
    child_vm->vm_end = parent_vm->vm_end;
    child_vm->vm_mmsz = parent_vm->vm_mmsz;
    child_vm->vm_next = NULL;
    child_vm->vm_file = parent_vm->vm_file;
    child_vm->vm_flags = parent_vm->vm_flags;
    child_vm->vm_offset = parent_vm->vm_offset;

 /******************* I AM TRYING TO LOAD HERE IT SELF ************************/
  uint64_t* pte = Pte((uint64_t *)parent_vm->vm_start);
  if((*pte & PRESENT) == 0) // if frame is not allocated in pm
     {
       uint64_t* frame = pmmngr_alloc_block();
       if(frame == 0)
        {
          printf("I can't allocate here good bye\n");
          return ;   // frame allocation failed
       }
        *pte =(uint64_t)frame | PRESENT | WRITE | USER;
     }
     int j;
     for(j=0; j<parent_vm->vm_mmsz; j++)
     {
      *((uint64_t *)(parent_vm->vm_start + j)) = *((uint64_t *)((uint64_t)parent_vm->vm_file + parent_vm->vm_offset + j));
     }
    parent_vm = parent_vm->vm_next;   
 }

 child_pcb->heap_vma = (struct vm_area_struct *)kmalloc(1); // allocate a separate page for heap vma, however this is waste
 struct vm_area_struct *tmp = child_pcb->mm->mmap;
 while(tmp->vm_next != NULL)  tmp = tmp->vm_next;  // go to the last vma
 child_pcb->heap_vma->vm_start = child_pcb->heap_vma->vm_end = ALIGN_DOWN((uint64_t)(tmp->vm_end + 0x1000));  // start from next page (keep the distance)
 child_pcb->heap_vma->vm_mmsz = 0x1000;
 uint64_t* pte = Pte((uint64_t *)child_pcb->heap_vma->vm_start);
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

/* switch back to caller address space before we return */
    asm volatile("movq %0, %%cr3":: "b"(parent_pcb->pml4e));
//printf("%d Process count\n",process_count);   
//asm volatile("hlt"); 
}

/************ This is crazy function Printf does something bad and index gets modified so be careful when u add any printf ******/ 
int fork()
{
 uint64_t rsp;
 uint64_t *rsp1;
 __asm__(
         "movq %%rsp, %0;"
         :"=g"(rsp)
        );

 rsp1 = (uint64_t *)rsp;
 int i;

//for(i=0;i<50;i++) 
//  printf(" %d:%p ",i,rsp1[i]); 

//asm volatile("hlt");
// printf("\n");
 child_ret_addr = rsp1[17];
 child_stack_addr = rsp1[20]; 

 for(i=0;i<14;i++) //reg[0] %rbx,reg[1] %rcx ....reg[14] %r15 
  reg[i] = rsp1[17-i-1]; 
// printf("%p:%p %p:%p \n",rsp1[22],child_ret_addr,rsp1[25],child_stack_addr);
//  for(i=7;i<21;i++) 
//    printf("%p %p\n",rsp1[21+7-i],reg[i-7]);
 
 //printf("%p %p\n",rsp,rsp1);
 fork_process();   
// printf("%d\n",process_count);
// for(i=1;i<=process_count;i++)
 //  printf("%p \n",ready_queue[i]);   
 //  asm volatile("hlt");   
 return child_pid;
}

void sleep_process()
{
  struct task_struct *pcb = get_current_process();   
  // save current rsp  
  __asm__(
          "movq %%rsp, %0;"
          :"=g"(pcb->rsp)
         );
  // Load kernel's rsp
   __asm__(
           "movq %0, %%rsp;"
            :
            :"r"(((struct task_struct *)ready_queue[0])->rsp)
          );
   //load kernels cr3
   asm volatile("movq %0, %%cr3":: "b"(((struct task_struct *)ready_queue[0])->pml4e));

   first_switch = 1;
   asm volatile("pop %rbx");  // extra reg schedule;
   asm volatile("pop %rax");  // isr next instruction
   asm volatile("jmp *%rax");
}

int sleep(int t)
{
  struct task_struct *pcb = get_current_process();
  pcb->sleep_time = t; 
//   printf("sleep time %d \n",t);  
//   asm volatile("hlt"); 
  // save current process rsp
  sleep_process();   

  return t;  
}


/************************************************************************
 int        execvpe(pathname,argv,envp);
 char       *pathname;                   Path name of file to be executed
 char       *argv[];                     Array of pointers to arguments
 char       *envp[];                     Array of pointers to environment
**************************************************************************/
uint64_t execvp(uint64_t arg1,uint64_t arg2, uint64_t arg3)
{

/*************** use kmalloc to access it further when u switch the stack  *******/ 

 char *pathname = (char *)arg1;
//  char **argv = (char **)arg2;
//  char **envp = (char **)arg3;

//printf("%s",pathname);
//asm volatile("hlt");
/*
  PID, PPID will be same 
  memory area will be different
  remove from the run queue and put at the same as run queue.
  don't change the process count
*/
struct task_struct *current_pcb =  get_current_process();

struct task_struct * pcb = (struct task_struct *)kmalloc(sizeof(struct task_struct));

 if(!pcb)
  return 0;

//printf("execv process %p\n",pcb);   

//put the path in temporary kernel space   
//printf("\npathname %s size %d\n",pathname,strlen(pathname));
char *tmp_path = (char *)kmalloc(strlen(pathname));
memcpy(tmp_path,pathname,strlen(pathname));


// find the current process in ready_queue
int i;
for(i=1;i<=process_count;i++)
{
  if(ready_queue[i] == (uint64_t)current_pcb)
    break;
}
 //ready_queue[i] = (uint64_t)pcb;
// printf("  %d ",i);
// asm volatile("hlt");
 pcb->mm = (struct mm_struct *)((char *)(pcb + 1));
 pcb->mm->count = 0;
 pcb->mm->mmap = NULL;

//fill the entry of pcb
 pcb->pid =  current_pcb->pid;
 pcb->ppid = current_pcb->ppid; 
 pcb->zombie = 0; // this is not zombie
 pcb->iswait = current_pcb->iswait; // if your clone was waiting for some1 you should wait  
 pcb->readwait = current_pcb->readwait;  
 pcb->sleep_time = 0; // this is not sleeping
 pcb->cursor = 0;  // read 
 pcb->prev_cursor = 0; //write
 memcpy(pcb->pname,pathname ,strlen(pathname));  
 uint64_t *pml4e =  kmalloc(512);
int j;
for(j=0;j<512;j++)
  pml4e[j] = 0;
pml4e[511] = *(Pml4e((uint64_t*)0xFFFFFFFF80200000)); //point to pdpe of kerne
pml4e[500] = ALIGN_DOWN((uint64_t)(*Pte(pml4e))) | PRESENT | WRITE; //self reference
pcb->pml4e = ALIGN_DOWN((uint64_t)(*Pte(pml4e)));

//my process stack grows upwards
 pcb->bmpPtr = (uint64_t *)BUMP_PTR_USER_SPACE;

/* we need to switch process address space in order to allocate its stack */

   // asm volatile( "cli");
    asm volatile("movq %0, %%cr3":: "b"(pcb->pml4e));
  //  asm volatile( "sti");

// printf("bump ptr %p\n",pcb->bmpPtr);   //currently stack is being allocated static
 pcb->stack = (uint64_t *)vmmngr_bump_alloc_process(&pcb->bmpPtr,PAGE_SIZE);  //lets allocate two page


// printf("bump ptr %p stack %p \n",pcb->bmpPtr,pcb->stack);
 pcb->kstack[511] = 0x23; //ss
 pcb->kstack[510] = (uint64_t)(&(pcb->stack[511])); //rsp
 //printf("%p %p %p...kstack\n",pcb->kstack[510],pcb->stack,&(pcb->stack[1]));
// asm volatile("hlt");
 pcb->kstack[509] = 0x200286; //rflags
 pcb->kstack[508] = 0x1b; //cs
 pcb->kstack[507] = 0;  //rip entry point scroll down to see the value

 pcb->kstack[506] = 1; pcb->kstack[505] = 2;  pcb->kstack[504] = 3;  pcb->kstack[503] = 4;
 pcb->kstack[502] = 5; pcb->kstack[501] = 6;  pcb->kstack[500] = 7;  pcb->kstack[499] = 8;
 pcb->kstack[498] = 9; pcb->kstack[497] = 10; pcb->kstack[496] = 11; pcb->kstack[495] = 12;
 pcb->kstack[494] = 13;pcb->kstack[493] = 14; pcb->kstack[492] = 15;
 pcb->kstack[491] = (uint64_t)(x86_64_isr_vector1 + 34);
 pcb->kstack[490] = 16; //for extra rbx
 pcb->rsp = &(pcb->kstack[490]);

 //printf("hello %p world %p proc3 %p proc4 %p\n", search("bin/hello"),search("bin/world"),search("bin/proc3"),search("bin/proc4"));
 //asm volatile("hlt");

// printf("pathname %s tmp_path %s\n",pathname,tmp_path);
// asm volatile("hlt");  
 struct posix_header_ustar *file;
 file = (struct posix_header_ustar *)search(tmp_path);
// printf("%s %p file\n",pathname, file);
 file = file + 1; // point to the elf header
 struct ELF *elf =(struct ELF *)file;
 //now i have elf header lets play with it
//lets get the section header from elfheader by using the offset of elfheader
 struct ELF_ph * elfph = (struct ELF_ph *)((uint64_t)elf + elf->e_phoff);
// printf("elfphnum %d\n",elf->e_phnum);
 for(i=0;i<elf->e_phnum;i++) {
  if(elfph->p_type ==  PT_LOAD)
   {
    //  printf("We need to load this segment %d %d %d\n",elfph->p_vaddr,elfph->p_memsz,elfph->p_offset);
     uint64_t* pte = Pte((uint64_t *)elfph->p_vaddr);
     if((*pte & PRESENT) == 0) // if frame is not allocated in pm
     {
       uint64_t* frame = pmmngr_alloc_block();
       if(frame == 0)
        {
          printf("I can't allocate here good bye\n");
          return 0;   // frame allocation failed
       }
        *pte =(uint64_t)frame | PRESENT | WRITE | USER;
     }
     int j;
     for(j=0; j<elfph->p_memsz; j++)
     {
      *((uint64_t *)(elfph->p_vaddr + j)) = *((uint64_t *)((uint64_t)elf + elfph->p_offset + j));
     }
    struct vm_area_struct *vm;
    vm = malloc_vma(pcb->mm);
    vm->vm_start = elfph->p_vaddr;
    vm->vm_end = vm->vm_start + elfph->p_memsz;
    vm->vm_mmsz = elfph->p_memsz;
    vm->vm_next = NULL;
    vm->vm_file =(uint64_t)elf;
    vm->vm_flags = elfph->p_flags;
    vm->vm_offset = elfph->p_offset;
   // printf("start %p end %p size %d\n",vm->vm_start,vm->vm_end,vm->vm_mmsz);
 }
  elfph= elfph + 1;
 }
 // printf("Process loaded_1\n");
//  asm volatile("hlt"); 

 pcb->heap_vma = (struct vm_area_struct *)kmalloc(1); // allocate a separate page for heap vma, however this is waste
// printf("Process loaded_1_1_1 %p %p %p %d\n",pcb->heap_vma,pcb->heap_vma->vm_start,pcb->heap_vma->vm_end,pcb->heap_vma->vm_mmsz);
 struct vm_area_struct *tmp = pcb->mm->mmap;
 //printf("tmp %p\n",tmp);   
 while(tmp->vm_next != NULL)  tmp = tmp->vm_next;  // go to the last vma
// printf("%p..\n",tmp);  
 pcb->heap_vma->vm_start = pcb->heap_vma->vm_end = ALIGN_DOWN((uint64_t)(tmp->vm_end + 0x1000));  // start from next page (keep the distance)
 //printf("Process loaded_1_1_3_4_\n");
 pcb->heap_vma->vm_mmsz = 0x1000;
 //printf("Process loaded_1_1\n");
 uint64_t* pte = Pte((uint64_t *)pcb->heap_vma->vm_start);
 //printf("Process loaded_1_2\n");
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
  //printf("Process loaded_2\n");
  //asm volatile("hlt");


// printf("%p %p\n",pcb->heap_vma->vm_start,pcb->heap_vma->vm_end);
 pcb->kstack[507] = (uint64_t)elf->e_entry; //RIP
// printf("pcb %p %p %p \n",pcb->heap_vma->vm_start, pcb->heap_vma->vm_end ,tmp->vm_end);
// printf("entry %p  stack %p\n", elf->e_entry,pcb->kstack[507]);
 /* switch back to kernel address space before we return */
 // asm volatile( "cli");
  //  asm volatile("movq %0, %%cr3":: "b"(kernel_pml4e));
 // asm volatile( "sti");
//  printf("Process loaded\n");  
//  printf("process_count %d\n",process_count);
//  for(i=1;i<=process_count;i++)
//   printf("pcb %d %p\n",i,ready_queue[i]); 

//  asm volatile("hlt");  

/*   asm volatile("pop %rbx");  // extra rbx pop
   asm volatile("pop %rax");  // get the isr addr
   asm volatile("jmp *%rax");
*/

// exit_process(0);
 ready_queue[++process_count] = (uint64_t)pcb;  
 exit_process(0);
 //printf("process count %d\n",process_count);   
 //asm volatile("hlt");   
  return 0;
}

int waitpid(int pid, int status)
{
  struct task_struct *pcb = get_current_process();
  struct task_struct *child_pcb;
  uint64_t ppid = pcb->pid;
  int i;
  if(pid<0) //find any child and check for the status
   {
    for(i=1;i<=process_count;i++)
    {
     child_pcb = (struct task_struct *)ready_queue[i];
     if(child_pcb->ppid == ppid )  //
      {
        pcb->iswait=child_pcb->pid;
          break; 
      }
    }
   if(i>process_count) //no child process found go and resume execution
    {
      status = 0;
      pcb->iswait = 0;   // remove the wait
      return 0;
    }
   else if(child_pcb->zombie) // child is already dead
    {
      status = zombie_queue[child_pcb->pid];
      pcb->iswait = 0; // remove the wait ;
      return status;
    }
  else
    {
      pcb->iswait = 1;
      asm("int $0X81;"); //yield
      return 0;  //just return it won't schedule
    }
  
  }
  else // you have the pid find the child /
   {
    for(i=1;i<=process_count;i++)
    {
      child_pcb = (struct task_struct *)ready_queue[i];
      if(child_pcb->pid == pid)  // found the child
      {
        pcb->iswait = child_pcb->pid;
        break; 
      }
    }
   if(i>process_count) //no child process found go and resume execution
    {
      status = 0;
      pcb->iswait = 0;   // remove the wait
      return 0;
    }
   else if(child_pcb->zombie) // child is already dead
    {
      status = zombie_queue[child_pcb->pid];
      pcb->iswait = 0; // remove the wait ;
      return status;
    }
  else
    {
      pcb->iswait = 1;
      asm("int $0X81;"); //yield
      return 0;  //just return it won't schedule
    }
   }
}





uint64_t read_process(uint64_t fd, uint64_t buff, uint64_t count)
{
 // printf(" foreground process %d\n",foreground_process);   
 // asm volatile("hlt");    
  struct task_struct *cpcb = get_current_process();    
  //cpcb->readwait = 1; // wait for the read unless finish 
  struct task_struct *pcb = get_process(foreground_process); 
  int i;
  char *buf = (char *)buff; 
  if(pcb==NULL)
  {
    printf("Foreground process not found\n");
    return 0; 
  } 
  if(fd == 0) //copy the content of pcb buffer into buffer  
  {
    for(i=0;i<count;i++) 
    {
     if(pcb->prev_cursor < pcb->cursor) { 
      buf[i] = pcb->buf[pcb->prev_cursor++];
      if(buf[i]=='\n')
      {
     //   printf("!!!%s\n",buf);
        buf[i] = '\0';
        return i;
      }
     }
     else
     {
       cpcb->readwait = 1; // wait the current proces untill read is finish    
       i--; 
       asm("int $0X81;"); //yield  
     }
    }
  }
  else if(fd == 1)  //not implementing this   
  {
   return 0;  
  }
  else if(fd == 2) //not implementing this    
  {
   return 0;
  }    
  else
  {
    printf("Bad source !!!\n");  
    return 0;  
  }      
//  printf("read halt\n");  
//  asm volatile("hlt");  
  return 0;
} 
uint64_t write_process(uint64_t fd, uint64_t buff, uint64_t count)
{
 if(count < 1) return 0;

 char *buf = (char *)buff;   
 if(fd == 1) 
 {
 int i;
 for(i=0;i<count;i++)
  putch(buf[i]);
 return i;  
//   printf("...%s\n",((get_process(foreground_process))->buf));
  printf("...%s\n",buf);
//  asm volatile("hlt"); 
//  for(i=0;i<count;i++)
//   putch(buf[i]); 
//  memset((get_process(foreground_process))->buf,'\0',1024);   
//  (get_process(foreground_process))->cursor = 0;    
 // return i;
 }  
 else if(fd == 2) //not implementing this
 {
   return 0;
 }
 else
 {
    printf("Bad source !!!\n");
    return 0;
 }
 return 0; 
}
/*
int opendir(char *name)
{

 return 0;
}   

int closedir(char *name)
{

 return 0;
} 
int readdir(char *name)
{

 return 0;
}

int openf(char *name)
{

 return 0;
}
*/

void setforeground()
{
 foreground_process = (get_current_process())->pid; 
}

void dp()
{
  struct task_struct *pcb;
  int i;
  for(i=1;i<=process_count;i++)
  {
    pcb = (struct task_struct *)ready_queue[i];
    printf("%s pid:%d ppid:%d\n", pcb->pname,pcb->pid,pcb->ppid); 
  } 
}
