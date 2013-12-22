#include <defs.h>
#include <sys/gdt.h>
#include <sys/tarfs.h>
#include <system.h>

#define INITIAL_STACK_SIZE 4096
char stack[INITIAL_STACK_SIZE];
uint32_t* loader_stack;
extern char kernmem, physbase;


struct page_table Page_Table; 
uint32_t freePage[BLOCKS];

void start(uint32_t* modulep, void* physbase, void* physfree)
{       
  int i;
    struct smap_t {
                uint64_t base, length;
                uint32_t type;
        }__attribute__((packed)) *smap;
      
  
        // initially Mark all pages used  
        mm_set(freePage,MEM_SIZE); 
 
        while(modulep[0] != 0x9001) modulep += modulep[1]+2;
        for(smap = (struct smap_t*)(modulep+2); smap < (struct smap_t*)((char*)modulep+modulep[1]+2*4); ++smap) {
            if (smap->type == 1 /* memory */ && smap->length != 0) {
                        // Mark the memory available in the free list 
                        pmmngr_init_region(smap->base, smap->length);      
                  }
        }

      /* we need to mark the kernel memory as used kernel starts from 200000 and end at 220000 */ 
      /* so we will mark the memory till 400000 as used however we can keep it till 220000 as well */ 
      pmmngr_uninit_region((uint64_t)0,(uint64_t)0x400000);  // mark all kernel memory as used 
      
      /* we need to map the kernel memory to the page table */
      
      // initialize the page table
      set_page_table(&Page_Table);

     uint64_t *addr = (uint64_t*)physbase; 
    /* we need to map the kernel memory to the page table */ 
    for(addr = (uint64_t*)physbase; addr<=(uint64_t*)physfree; addr++)  
      {
       vmmngr_map_page(addr,(uint64_t *)((uint64_t)addr + KERNEL_VIRTUAL_BASE),&Page_Table);            
      }
   
 
      /* Map the Video memory This will map to single page 80*25*2 < 1 page */
      vmmngr_map_page((uint64_t *)0xb8000,(uint64_t*)VIDEO_VIRTUAL_MEMORY,&Page_Table);  
    //  printf("Video Memory has been marked in virtual address space\n"); 

      /* pass the pml4e value to cr3 register */
      kernel_pml4e = (uint64_t *)ALIGN_DOWN((uint64_t)Page_Table.root);
      write_cr3(&Page_Table);
      init_video();

/********************************* KERNEL CREATION ********************************/
 struct task_struct *pcb0 = (struct task_struct *)kmalloc(sizeof(struct task_struct));  //kernel 
 pcb0->pml4e =(uint64_t)kernel_pml4e;  // kernel's page table   
 pcb0->pid = 0;  // I'm kernel init process  so pid 0  
 pcb0->iswait = 0; //not waiting for any one
 pcb0->stack =(uint64_t *)stack; //my stack is already created by prof :)  
 process_count = 0; //at this point we don't have any other process in ready_queue so go ahead and create one and update this 
 sleeping_process_count = 0; // at this point no body is sleeping 
 // initialize processes queue  
 for(i=0;i<100;i++)  {   
  zombie_queue[i] = 0;    // no process is zombie 
 }
 foreground_process = 3; // process with this pid will be foreground process  
 
  // put them in the ready queue
  ready_queue[0] =(uint64_t ) pcb0;  //kernel 

/*
char fname[] = "bin/hello";
malloc_pcb(fname);

char fname1[] = "bin/world";
malloc_pcb(fname1);

char fname2[] = "bin/proc3";
malloc_pcb(fname2);

char fname3[] = "bin/proc4";
malloc_pcb(fname3);    
*/
/*************************************** Please change here fname ******************/
char fname4[] = "bin/printf";
malloc_pcb(fname4); 

  idt_install();
  __asm__("sti");

  //init_context_switch();    
  asm volatile("mov $0x2b,%ax");
  asm volatile("ltr %ax");
  tarfs_init();    
  while(1);      
}


#define INITIAL_STACK_SIZE 4096
char stack[INITIAL_STACK_SIZE];

uint32_t* loader_stack;
extern char kernmem, physbase;

void boot(void)
{
	// note: function changes rsp, local stack variables can't be practically used
//	register char *temp1, *temp2;
	__asm__(
		"movq %%rsp, %0;"
		"movq %1, %%rsp;"
		:"=g"(loader_stack)
		:"r"(&stack[INITIAL_STACK_SIZE])
	);
       init_video(); 
        reload_gdt();
	setup_tss();
	start(
		(uint32_t*)((char*)(uint64_t)loader_stack[3] + (uint64_t)&kernmem - (uint64_t)&physbase),
		&physbase,
		(void*)(uint64_t)loader_stack[4]
	);
	while(1);
}
