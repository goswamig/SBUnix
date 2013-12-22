//Copyright ©2013, by Gautam Kumar, Haseeb Niaz, Krishna Kanhaiya 

// reference forum.openvz.org
#ifndef _SYSTEM_H
#define _SYSTEM_H
#include<defs.h>
#include<sys/tarfs.h>  
void idt_set_gate(int num, uint64_t base, uint16_t sel, uint16_t typer);
void idt_install();
void* memset(void *ptr, int c, int n);
void printf(const char *format, ...);
void scanf(char* str, void* buf);
void putchar(char ch);



struct task_struct; // forward declaration  
typedef uint64_t size_t;   

//Hexadecimal hash
//char HASH[17]="0123456789abcdef";

#define va_start(v,l)     __builtin_va_start(v,l)
#define va_arg(v,l)       __builtin_va_arg(v,l)
#define va_end(v)         __builtin_va_end(v)
#define va_copy(d,s)      __builtin_va_copy(d,s)
typedef __builtin_va_list va_list;

#define true 1
#define false 0

/************************** utility function **************************/
extern void *memcpy(void *dest, const void *src, int count);
extern void *memset(void *dest, int val, int count);
extern unsigned short *memsetw(unsigned short *dest, unsigned short val, int count);
extern int strlen(const char *str);
extern int strcmp(const char *str1, const char *str2); 
char* strcpy(char *dst, const char *src); 
void itoa(long int value, char *out, int radix);
int  atoi(char *str);
int  oct2dec(int n);
int  pow(int a, int b); // calculate a^b
uint64_t stoi(char *s); // the message and then the line #
int strncmp(const char *p, const char *q, size_t n); 

/* CONSOLE.C */
extern void init_video(void);
extern void puts(const char *text);
extern void putch(unsigned char c);
extern void cls();
void putaddr(unsigned long num);
void puthex(unsigned int num);
extern unsigned char inportb (int _port);
extern void outportb (int _port, unsigned char _data);


/* Memory Managment  stuff*/
#define PRESENT   1
#define WRITE     2
#define USER      4


#define MEM_SIZE 134209536
#define PAGE_SIZE 4096
#define BLOCKS 1024   // MEM_SIZE/(4096*32)



#define RECURSIVE_SLOT (500L)


//http://forum.osdev.org/viewtopic.php?f=1&p=176913

#define L4_SHIFT (39)
#define L3_SHIFT (30)
#define L2_SHIFT (21)
#define L1_SHIFT (12)

#define UPPER_ADDR(x) ((uint64_t*)(0xffffL<<48|(x)))

#define PGTBL_ADDR UPPER_ADDR((RECURSIVE_SLOT<<L4_SHIFT))

#define PGDIR_ADDR UPPER_ADDR((RECURSIVE_SLOT<<L4_SHIFT) \
                     |(RECURSIVE_SLOT<<L3_SHIFT))

#define PDPT_ADDR UPPER_ADDR((RECURSIVE_SLOT<<L4_SHIFT) \
                     |(RECURSIVE_SLOT<<L3_SHIFT) \
                     |(RECURSIVE_SLOT<<L2_SHIFT))

#define PML4T_ADDR UPPER_ADDR((RECURSIVE_SLOT<<L4_SHIFT) \
                     |(RECURSIVE_SLOT<<L3_SHIFT) \
                     |(RECURSIVE_SLOT<<L2_SHIFT) \
                     |(RECURSIVE_SLOT<<L1_SHIFT))



#define PAGE_TABLE_ALIGNLENT 0x1000

/* mask out bits under page size */
#define ALIGN_DOWN(x)   (x & ~(PAGE_TABLE_ALIGNLENT-1))



//mapping of kernel and other physical memory to virtual memory 
#define KERNEL_VIRTUAL_BASE    0xFFFFFFFF80000000 //kernel always go higher PM of kernel is 200000 to 220000 
#define VIDEO_VIRTUAL_MEMORY   0xFFFFFFFF80300000 // Anything above kernel addr space i.e > 0xFFFFFFFF90020000
#define BUMP_PTR               0xFFFFFFFF80700000 // kernels bump ptr 
#define BUMP_PTR_USER_SPACE    0xffffff8000000000 - 512 //user space bump ptr 

//A structure pointing to the top of the page hierarchy 
struct page_table {
uint64_t *root;
};  
uint64_t * kernel_pml4e; // store the kernel's page table pml4e so that we can switch to kernel anytime using this :-) 

void write_cr3(struct page_table *st);

void mm_set(uint32_t* freePage,uint64_t mem_size); 
inline char  mmap_test (int bit); 
inline void mmap_set (int bit);
inline void mmap_unset (int bit); 
int mmap_first_free ();
void pmmngr_init_region (uint64_t base, uint64_t size);
void pmmngr_uninit_region (uint64_t base, uint64_t size);
void pmmngr_free_block (uint64_t* p); 
uint64_t * pmmngr_alloc_block ();
void PageInfo();
void vmmngr_map_page (uint64_t* phys, uint64_t* virt,struct page_table*);   
void set_page_table(struct page_table*);
uint64_t * vmmngr_bump_alloc (uint64_t size); 
uint64_t * vmmngr_bump_alloc_process(uint64_t **bmpPtr,uint64_t size);   
uint64_t *kmalloc(uint64_t size); 
void self_reference_pml4e(struct page_table *base_table);    
uint64_t *pml4e_t(uint64_t *addr); 
uint64_t *pdpe_t(uint64_t *addr); 
uint64_t *pde_t(uint64_t *addr); 
uint64_t *pte_t(uint64_t *addr);  
/************* following function perform virtual addr to virtual addr mapping and creates the entry in page table ***********/ 
uint64_t* Pml4e(uint64_t* addr);   
uint64_t* Pdp(uint64_t* addr);   
uint64_t* Pdpe(uint64_t* addr);   
uint64_t* Pd(uint64_t* addr);   
uint64_t* Pde(uint64_t* addr);   
uint64_t* Pt(uint64_t* addr);  
uint64_t* Pte(uint64_t* addr); 

/********************** ELF header *******************/
//reference https://build.opensuse.org
/* constants are for the segment types stored in the image header 
 which will map to the elf type 
*/
#define EI_NIDENT (16)
#define PT_NULL   0   /* p_type */
#define PT_LOAD   1
#define PT_DYNAMIC  2
#define PT_INTERP 3
#define PT_NOTE   4
#define PT_SHLIB  5
#define PT_PHDR   6
#define PT_TLS    7

/* segment permission bit */ 
// reference http://uw714doc.sco.com/en/SDK_cprog/_Segment_Permissions.html
#define PF_X      0x1  //Execute
#define PF_W      0x2  //write
#define PF_R      0x4  //read


//elf header 

struct ELF 
{
  unsigned char e_ident[EI_NIDENT]; /* Magic number and other info */
  uint16_t  e_type;         /* Object file type */
  uint16_t  e_machine;      /* Architecture */
  uint32_t  e_version;      /* Object file version */
  uint64_t  e_entry;        /* Entry point virtual address */
  uint64_t  e_phoff;        /* Program header table file offset */
  uint64_t  e_shoff;        /* Section header table file offset */
  uint32_t  e_flags;        /* Processor-specific flags */
  uint16_t  e_ehsize;       /* ELF header size in bytes */
  uint16_t  e_phentsize;    /* Program header table entry size */
  uint16_t  e_phnum;        /* Program header table entry count */
  uint16_t  e_shentsize;    /* Section header table entry size */
  uint16_t  e_shnum;        /* Section header table entry count */
  uint16_t  e_shstrndx;     /* Section header string table index */
};

// ELF Program Headers    
  struct ELF_ph{
   uint32_t p_type;                  /* type of segment */
   uint32_t p_flags;                 /* segment attributes */
   uint64_t p_offset;                /* offset in file */
   uint64_t p_vaddr;                 /* virtual address in memory */
   uint64_t p_paddr;                 /* reserved */
   uint64_t p_filesz;                /* size of segment in file */
   uint64_t p_memsz;                 /* size of segment in memory */
   uint64_t p_align;                 /* alignment of segment */
};

void *search (char *fname);  // search the file into tarfs dir, return the tarfs pointer
struct task_struct * elf_parse(char *fname); // map the elf entries
void map_vma(struct ELF *elf, struct ELF_ph *elfph, struct task_struct *pcb); // map and load all segment into pcb 

//tarfs_entry tarfs_fs[100];
int get_per_ind(char* dir);
int get_per_ind_file(char* dir);
uint64_t is_file_exists(char* filename);
/*void tarfs_init();
uint64_t open_dir(char * dir);
uint64_t read_dir(uint64_t dir);
uint64_t open(char * file);
int read_file(uint64_t file_addr, int size, uint64_t buf);
*/

/***************** MM and VMA structure ************/

/*  VMA struct */
struct vm_area_struct{
    struct mm_struct * vm_mm;       /* associated mm_struct */
    uint64_t vm_start;              /* VMA start, inclusive */
    uint64_t vm_end;                /* VMA end, exclusive */
    uint64_t vm_mmsz;               /* VMA size */ 
    unsigned long vm_flags;         /* flags */
    uint32_t grows_down;
    uint64_t vm_file;          /* mapped file, if any */
    struct vm_area_struct  *vm_next;/* list of VMA's */ 
    uint64_t vm_offset;    /* file offset */  
};


struct mm_struct {
	int count;
	uint64_t * pt; // page table pointer  
	unsigned long context;
	unsigned long start_code, end_code, start_data, end_data;
	unsigned long start_brk, brk, start_stack, start_mmap;
	unsigned long arg_start, arg_end, env_start, env_end;
	unsigned long rss, total_vm, locked_vm;
	unsigned long def_flags;
	struct vm_area_struct * mmap;
	struct vm_area_struct * mmap_avl;
};

struct vm_area_struct *malloc_vma(struct mm_struct *mm);   



/****************** Process **********************/ 
//PCB structure
struct task_struct{
uint64_t state;   //0 runnable ,-1 unrunnable, >0 stopped   
uint64_t counter; //how long its run;
uint64_t pid;    //my id;
uint64_t ppid;  //my parent pid   
uint64_t priority; // when should it run
uint64_t *stack; // its own stack 
uint64_t *rsp; // process stack pointer 
uint64_t pml4e; // will directly go to cr3 register 
uint64_t *bmpPtr; // user stack of the process
struct mm_struct *mm; // a pointer to mm_struct
uint64_t rip; // instruction pointer
uint64_t status; // exit status
uint64_t zombie; // mark as zombie process
uint64_t kstack[512];   
struct vm_area_struct *heap_vma;  // vma for heap
int sleep_time; 
int iswait;  //wait_pid it will store the pid for which i m waiting  
char buf[1024]; //buffer to read 
int  cursor; // current position in buf  
int prev_cursor; //last read happen 
int readwait;
char pname[100]; //process name
};


struct task_struct * malloc_pcb(void *fun_ptr);     
void fun1();
void fun2();
void schedule();
void run_process(struct task_struct **pcb);    
uint64_t ready_queue[100];  // can accomodate 100 process now 
uint64_t zombie_queue[100] ; // queue of all process which contains exit status of zombie process 
uint64_t sleep_queue[100];  // To keep track the time quantum 
void init_context_switch();
void sleep_process();   
/******************************************* SYSTEM CALL ************************************/ 
void exit_process(int i);  // exit process 
int get_pid();
int get_ppid();   
struct task_struct* get_current_process();
struct task_struct *get_process(uint64_t pid); 
void* malloc(uint64_t  size);   
int fork();  
void fork_process(); 
int sleep(int j);   
uint64_t execvp(uint64_t arg1,uint64_t arg2, uint64_t arg3);  
int waitpid(int,int);   
uint64_t read_process(uint64_t fd, uint64_t buf, uint64_t count);   
uint64_t write_process(uint64_t fd, uint64_t buf, uint64_t count);    
uint64_t  opendir(char *name);  
uint64_t closedir(char *name);
uint64_t readdir(uint64_t name);      
uint64_t openf();
int readf(uint64_t,int,uint64_t);
void setforeground();
void dp();

uint64_t foreground_process;   
uint64_t sleeping_process_count;  // keeps track the number of sleeping process 
uint64_t process_count; // keep tracks the number of current process in ready_queue(run_queue)  
#define switch_to(prev,next) do {\
        __asm__( "pushq %rax");\
        __asm__( "pushq %rbx");\
        __asm__( "pushq %rcx");\
        __asm__( "pushq %rdx");\
        __asm__(\
                 "movq %%rsp, %0;"\
                :"=g"(prev->rsp)\
               );\
       asm volatile("movq %0, %%cr3":: "b"(next->pml4e));\
       __asm__(\
                "movq %0, %%rsp;"\
                :\
                :"r"(next->rsp)\
               );\
        __asm__( "popq %rdx");\
        __asm__( "popq %rcx");\
        __asm__( "popq %rbx");\
        __asm__( "popq %rax");\
} while (0)



/* Port */
//void outb(unsigned short _port, unsigned char _data);
void outb(uint16_t port, char data);
unsigned char inb(unsigned short _port);

struct regs
{
    unsigned int gs, fs, es, ds;
    unsigned int edi, esi, ebp, esp, ebx, edx, ecx, eax;
    unsigned int int_no, err_code;
    unsigned int eip, cs, eflags, useresp, ss;    
};


#endif
