/* copyright © 2013 by Gautam Kumar, Haseeb Niaz, Krishna Kanhaiya */
#include <defs.h>
#include <system.h>
struct idt_entry
{
    uint16_t offset_0_15;
    uint16_t selector;
    unsigned ist : 3 ;
    unsigned reserved0 : 5;
    unsigned type : 4;
    unsigned zero : 1;
    unsigned dpl : 2;
    unsigned p : 1;
    uint16_t offset_16_31;
    uint32_t offset_63_32;
    uint32_t reserved1;
} __attribute__((packed));

struct idt_ptr
{
    uint16_t limit;
    uint64_t base;
} __attribute__((packed));


struct idt_entry idt[256];
struct idt_ptr idtp;

extern void x86_64_isr_vector0();
extern void x86_64_isr_vector1(); 
extern void x86_64_isr_vector2();
extern void x86_64_isr_vector3();  //putchar
extern void x86_64_isr_vector4();  //yield  
extern void x86_64_isr_vector13(); // gpf handler 
extern void x86_64_isr_vector14(); // pf handler  


void _x86_64_asm_igdt(struct idt_ptr* idtp);

void idt_set_gate(int num, uint64_t base, uint16_t sel, uint16_t typer)
{
    idt[num].offset_0_15 = base & 0x0000ffff;
    idt[num].selector = sel; //0x08
    idt[num].ist = 0 ;
    idt[num].reserved0 = 0 ;
    idt[num].type = 0x0e;
    idt[num].zero = 0;
    idt[num].dpl = 0;
    if(num > 127)  // when s/w interrupt occurs 
     idt[num].dpl = 3; 
    idt[num].p = 1; 
    idt[num].offset_16_31 = ((base) >> 16) & 0x0000ffff;
    idt[num].offset_63_32 = ((base) >> 32) & 0xffffffff;
    idt[num].reserved1 = 0;
}

void irq_remap(void) {
    outb(0x20, 0x11);
    outb(0xA0, 0x11);
    outb(0x21, 0x20);
    outb(0xA1, 0x28);
    outb(0x21, 0x04);
    outb(0xA1, 0x02);
    outb(0x21, 0x01);
    outb(0xA1, 0x01);
    outb(0x21, 0x0);
    outb(0xA1, 0x0);
}


/* Installs the IDT */
void idt_install()
{

    memset(&idt, 0, sizeof(struct idt_entry) * 256);

    idtp.limit = (sizeof (struct idt_entry) * 256) - 1;
    idtp.base = (uint64_t)&idt;

    _x86_64_asm_igdt(&idtp);
    idt_set_gate(0, (uint64_t)&x86_64_isr_vector0, 0x08, 0x8e);
    idt_set_gate(13, (uint64_t)&x86_64_isr_vector13, 0x08, 0x8e); //gpf handler
    idt_set_gate(14, (uint64_t)&x86_64_isr_vector14, 0x08, 0x8e); //pf handleR
    idt_set_gate(32, (uint64_t)&x86_64_isr_vector1, 0x08, 0x8e);  //timer 
    idt_set_gate(33, (uint64_t)&x86_64_isr_vector2, 0x08, 0x8e); //key board  
    idt_set_gate(128, (uint64_t)&x86_64_isr_vector3, 0x08, 0x8e); /*putchar, exit, malloc, getpid, getppid etc syscall */   
     idt_set_gate(129, (uint64_t)&x86_64_isr_vector4, 0x08, 0x8e);
    //_x86_64_asm_igdt(&idtp);
    irq_remap();
}

