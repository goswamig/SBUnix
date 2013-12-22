/* copyright © 2013 by Gautam Kumar, Haseeb Niaz, Krishna Kanhaiya */
/* Refernce:  http://www.brokenthorn.com/Resources/OSDev17.html,
   http://www.ijack.org.uk,
   http://code.google.com/p/shovelos/source/browse/trunk/stage1.5/pt.h?r=72
   Discueed with samudra banerjee  
 */
#include <defs.h>
#include <system.h>


static uint32_t _mmngr_used_blocks = 0;
static uint32_t _mmngr_max_blocks = 0;
//static uint64_t _mmngr_memory_size;
static uint32_t *_mmngr_free_page;  // a bit vector  

uint64_t *pml4e=NULL;
uint64_t *pdpe=NULL;
uint64_t *pde=NULL;
uint64_t *pte=NULL;


void mm_set(uint32_t* freePage,uint64_t mem_size) {
  int bit; 
 _mmngr_free_page = (uint32_t *)freePage;
 _mmngr_used_blocks = _mmngr_max_blocks = mem_size/PAGE_SIZE;
 for(bit = 0; bit < _mmngr_max_blocks; bit++)
   mmap_set(bit);
}   

inline char  mmap_test (int bit) {
 
 return _mmngr_free_page[bit / 32] &  (1 << (bit % 32));
}

inline void mmap_set (int bit) {
 
  _mmngr_free_page[bit / 32] |= (1 << (bit % 32));
}

inline void mmap_unset (int bit) {
 
  _mmngr_free_page[bit / 32] &= ~ (1 << (bit % 32));
}

int mmap_first_free () {
        uint32_t i,j,bit; 
	//! find the first free bit
	for ( i=0; i< _mmngr_max_blocks / 32; i++)
		if (_mmngr_free_page[i] != 0xffffffff)
			for ( j=0; j<32; j++) {		//! test each bit in the dword
			        bit = 1 << j;
				if (! (_mmngr_free_page[i] & bit) )
					return i*32+j;
			}
	return -1;
}

/* mark the all pages as free from base to base+len */ 
void pmmngr_init_region (uint64_t base, uint64_t size) {
 
	int align = base / PAGE_SIZE;
	int blocks = size / PAGE_SIZE;
 
	for (; blocks>0; blocks--) {
		mmap_unset (align++);
		_mmngr_used_blocks--;
	}
 
	mmap_set (0);	//first block is always set. This insures allocs cant be 0
}

/* Mark all the pages as used from base to base+len */ 
void pmmngr_uninit_region (uint64_t base, uint64_t size) {
 
        int align = base / PAGE_SIZE;
        int blocks = size / PAGE_SIZE;

        for (; blocks>0; blocks--) {
                mmap_set (align++);
                _mmngr_used_blocks++; 
        }

        mmap_set (0);   //first block is always set. This insures allocs cant be 0
}

uint64_t * pmmngr_alloc_block () {

         
	if (_mmngr_max_blocks == _mmngr_used_blocks )
		return 0;	//out of memory
 
	int frame = mmap_first_free ();
 
	if (frame == -1)
		return 0;	//out of memory
 
	mmap_set (frame);
 
	uint64_t addr = frame * PAGE_SIZE; 
	_mmngr_used_blocks++;
 
	return (uint64_t*)addr;
}

void	pmmngr_free_block (uint64_t* p) {
 
	uint64_t addr = (uint64_t)p;
	int frame = addr / PAGE_SIZE;
 
	mmap_unset (frame);
 
	_mmngr_used_blocks--;
}

void PageInfo()
{

 printf("Total Blocks: %d, Free Blocks: %d, Used Blocks: %d\n",_mmngr_max_blocks,_mmngr_max_blocks - _mmngr_used_blocks ,_mmngr_used_blocks);     
}


void self_reference_pml4e(struct page_table *base_table)
{
  *(base_table->root + 500) = (uint64_t ) base_table->root|PRESENT | WRITE; 
}

void write_cr3(struct page_table *pt)
{
  uint64_t cr3 = ALIGN_DOWN((uint64_t)pt->root);
 // printf("cr3  %p\n",cr3); 
   __asm__("cli");
  asm volatile("movq %0, %%cr3":: "b"(cr3));
}


/* Map the pm to vm using Page table */
void vmmngr_map_page (uint64_t* phys, uint64_t* virt,struct page_table *base_table)
{

 //printf("phy %p   virt %p\n",phys,virt);
      
  if(!(pml4e=base_table->root))  // all will be NULL 
  {
    pml4e =base_table->root= pmmngr_alloc_block();
    if(pml4e == 0)
      return ;    // allocation for page table failed
  }

 self_reference_pml4e(base_table);

 // printf("pml4e_1 %p\n",pml4e);  
  //we have the pml4e ready point it to the pdpe  
  pml4e += 0x1ff & ((uint64_t)virt >> L4_SHIFT);
  //printf("pml4e_2 %p\n",pml4e);  
   
  // Now pdpe is present only when present bit is set in pml4e  
  if( (*pml4e & PRESENT) == 0) 
  {                                      // so pdpe is absent create a new one   
    pdpe = pmmngr_alloc_block();
    if(pdpe == 0)
     return;  // allocation for page table failed
    else
     *pml4e = (uint64_t)pdpe ; //point the pml4e+offset to the base of pdpe; 
  } 
  else // we need to assign right pdpe address from pml4e(pml4e is already pointing to right pdpe we just need to assign right(remove lower bit) addr 
  {
    pdpe = (uint64_t *)ALIGN_DOWN(*pml4e);   
  }
 //    printf("*pml4e_1 %p pdpe %p\n",*pml4e,pdpe);  

  // at this point of time I have pml4e and pdpe ready, so set the present bit in pml4e
  *pml4e |= PRESENT | WRITE; 
// printf("*pml4e_2 %p pdpe %p\n",*pml4e,pdpe);


  // we have the pdpe ready point it to the pde 
  pdpe += 0x1ff & ((uint64_t)virt >> L3_SHIFT);
 // printf("*pml4e_3 %p pdpe %p\n",*pml4e,pdpe);

// now pde is present only when present bit is set in pdpe
  if( (*pdpe & PRESENT) == 0)
  {                                        // so pde is absent create a new one   
    pde = pmmngr_alloc_block();
    if(pde == 0)
     return;  // allocation for page table failed
    else
     *pdpe = (uint64_t)pde; // point the pdpe+offset to the pde
  } 
  else   // we need to assign right pde address from pdpe after clearing the last 12 bits or get the base address for pde   
  {
    pde = (uint64_t *)ALIGN_DOWN(*pdpe);
  } 
   //printf("*pdpe_1 %p pde %p\n",*pdpe,pde);

  // at this point of time I have pdpe and pde ready, so set the present bit in pdpe 
  *pdpe |= PRESENT | WRITE;
  //printf("*pdpe_2 %p pde %p\n",*pdpe,pde);


  //we have the pde ready point it to the pte
  pde += 0x1ff &((uint64_t)virt >> L2_SHIFT);
  // printf("*pdpe_3 %p pde %p\n",*pdpe,pde);
  

//now pte is present only when present bit is set in pde
  if((*pde & PRESENT) == 0)
  {
    pte = pmmngr_alloc_block();
    if(pte == 0)
     return;   // allocation for page table failed   
    else
     *pde = (uint64_t)pte; // point the pde + offset to the pt 
  } 
  else   //we need to assign the right pt address from pde after clearing the last 12 bits or get the base address for pt 
  {
    pte = (uint64_t *)ALIGN_DOWN(*pde); 
  }
  //printf("*pde_1 %p pte %p\n",*pde,pte);
 
  // at this point of time I have pde and pte ready, so set the present bit in pde
  *pde |= PRESENT | WRITE;
   //printf("*pde_2 %p pte %p\n",*pde,pte);
 
  //we have the pte ready point it to the physical address 
  pte += 0x1ff & ((uint64_t)virt >> L1_SHIFT);
  //printf("*pde_3 %p pte %p\n",*pde,pte);
 
 
  *pte = ((uint64_t)phys/PAGE_SIZE)*PAGE_SIZE;      
  // printf("*pte %p\n",*pte);

  *pte |= PRESENT | WRITE; 
 //  printf("*pte %p\n",*pte);
  
/* printf("pml4e %p *pml4e %p\n",pml4e,*pml4e); 
  printf("pdpe %p *pdpe %p\n",pdpe,*pdpe);
  printf("pde %p *pde %p\n",pde,*pde);
  printf("pte %p *pte %p\n",pte,*pte);
*/
}
void set_page_table(struct page_table *root)
{
  root=NULL;
}

/*
 bump allocator for user process   

*/

uint64_t * vmmngr_bump_alloc_process (uint64_t **bumpPtr,uint64_t size) {
   uint64_t *ret,*tmp;
   uint64_t *frame;
   uint64_t no_of_blocks;

   no_of_blocks = size/PAGE_SIZE;

   if((size - (size/PAGE_SIZE)*PAGE_SIZE) != 0)
     no_of_blocks ++ ;

   ret = *bumpPtr - 512*no_of_blocks;
   tmp = ret;
 while(no_of_blocks)
   {
     no_of_blocks--;
     pte = Pte(tmp);
     if((*pte & PRESENT) == 0) // if frame is not allocated in pm
     {
       frame = pmmngr_alloc_block();
       if(frame == 0)
        return 0;   // frame allocation failed
       *pte =(uint64_t)frame | PRESENT | WRITE | USER; //update into page table
     }
     *tmp += 512;
   }
// printf("2: %p\n",bumpPtr);
  *bumpPtr = ret;
  return ret;
}




/*
  We have a bump allocator here. This is for kernel space.
  The pointer static uint64_t * bumpptr will be pointing to base of virtual memory intially
  Everything will be returned as multiple of page size (4KB)
  we are sure we will never get exhausted from 64 bit virtual address
  Kernel is mapped at higher level FFFF FFFF 8000 0000 and no address will reach here starting from 0
*/
uint64_t * vmmngr_bump_alloc (uint64_t size) {

   static uint64_t *bumpPtr=(uint64_t *)0xFFFFFFFF80700000;     // start vm from here 
   uint64_t *ret;
   ret = bumpPtr; 
   uint64_t *frame; 
   uint64_t no_of_blocks;

   no_of_blocks = size/PAGE_SIZE;

   if((size - (size/PAGE_SIZE)*PAGE_SIZE) != 0)
     no_of_blocks ++ ;

//  printf("1: %p\n",bumpPtr);
//  printf("blocks %d\n",no_of_blocks);

 while(no_of_blocks)
   {
     no_of_blocks--;
     pte = Pte(bumpPtr); 
     if((*pte & PRESENT) == 0) // if frame is not allocated in pm 
     {
       frame = pmmngr_alloc_block();
       if(frame == 0)
        return 0;   // frame allocation failed 
       *pte =(uint64_t)frame | PRESENT | WRITE; //update into page table  
     } 
     bumpPtr += 512; 
   }
 //  printf("2: %p\n",bumpPtr);
   return ret;
}

uint64_t *kmalloc(uint64_t size)
{
 return vmmngr_bump_alloc(size);
}
/*
 following function will take virtual address and it will return virtual address and when ever we need any entry of any other level 
 page table(pdpe, pde or pte) we will add pte) we will add.   
*/

uint64_t *pml4e_t(uint64_t *addr )
{
  return (uint64_t*) (PML4T_ADDR + (((uint64_t)addr>>L4_SHIFT)&0x1ff) ); 
} 

uint64_t *pdpe_t(uint64_t *addr)
{
 uint64_t pml4e_offset = (((uint64_t)addr>>L4_SHIFT)&0x1ff);
 uint64_t pdpe_offset =  (((uint64_t)addr>>L3_SHIFT)&0x1ff); 
 pml4e = pml4e_t(addr);
 if((*pml4e & PRESENT) == 0)      
 {                            //so pdpe is absent create a new one 
   pdpe = pmmngr_alloc_block();
   if(pdpe == 0)
    return 0;  // allocation for page table failed
   else
    *pml4e = (uint64_t)pdpe|PRESENT|WRITE; //point the pml4e+offset to the base of pdpe with right bits;
 }
                    // get the rite pdpe                   add offset to it 
 return (uint64_t *)(PDPT_ADDR + (pml4e_offset<<L1_SHIFT)) + pdpe_offset; 
}

uint64_t * pde_t(uint64_t *addr)
{
 uint64_t pml4e_offset = (((uint64_t)addr>>L4_SHIFT)&0x1ff);
 uint64_t pdpe_offset =  (((uint64_t)addr>>L3_SHIFT)&0x1ff);
 uint64_t pde_offset =  (((uint64_t)addr>>L2_SHIFT)&0x1ff);
 pdpe = pdpe_t(addr);
 if((*pdpe & PRESENT) == 0)
 {                            //so pde is absent create a new one
   pde = pmmngr_alloc_block();
   if(pde == 0)
    return 0;  // allocation for page table failed
   else
    *pdpe = (uint64_t)pde|PRESENT|WRITE; //point the pdpe+offset to the base of pde with right bits;
 }
                    //get the rite pde                            add the offset 
 return (uint64_t *)(PGDIR_ADDR + (pml4e_offset<<L2_SHIFT) + (pdpe_offset<<L1_SHIFT)) + pde_offset;   
}


uint64_t *pte_t(uint64_t *addr)
{
 uint64_t pml4e_offset = (((uint64_t)addr>>L4_SHIFT)&0x1ff);
 uint64_t pdpe_offset =  (((uint64_t)addr>>L3_SHIFT)&0x1ff);
 uint64_t pde_offset =  (((uint64_t)addr>>L2_SHIFT)&0x1ff);
 uint64_t pte_offset =  (((uint64_t)addr>>L1_SHIFT)&0x1ff);
  
 pde = pde_t(addr);
 if((*pde & PRESENT) == 0)
 {                            //so pde is absent create a new one
   pte = pmmngr_alloc_block();
   if(pte == 0)
    return 0;  // allocation for page table failed
   else
    *pde = (uint64_t)pte|PRESENT|WRITE; //point the pdpe+offset to the base of pde with right bits;
 }
 return (uint64_t *)(PGTBL_ADDR + (pml4e_offset<<L3_SHIFT) + (pdpe_offset<<L2_SHIFT) + (pde_offset<<L1_SHIFT)) + pte_offset;
}
/*
uint64_t *pte_t(uint64_t *addr)
{
     uint64_t pml4e_offset = (0x1ff&((uint64_t)addr>>39));
     uint64_t pdpe_offset  = (0x1ff&((uint64_t)addr>>30));
     uint64_t pde_offset   = (0x1ff&((uint64_t)addr>>21));
     uint64_t pte_offset   = (0x1ff&((uint64_t)addr>>12));
     uint64_t *pte;
     uint64_t *pde;
     uint64_t *pdpe;
     uint64_t *pml4e;
     int i;
     pml4e = (uint64_t*)((uint64_t)PML4T_ADDR + pml4e_offset);
     if((*pml4e & PRESENT) == 0)
     {
       pdpe = pmmngr_alloc_block();
       if(pdpe == 0)
         return 0;
       else
        *pml4e = (uint64_t)pdpe;
     }
     *pml4e = *pml4e | PRESENT | WRITE;
     pdpe =(uint64_t*)(((uint64_t)PDPT_ADDR + (pml4e_offset<<12))+ pdpe_offset);
     for(i=0;i<512;i++)
          pdpe[i] = 0;      

     if((*pdpe & PRESENT) == 0)
     {
       pde = pmmngr_alloc_block();
       if(pde == 0)
         return 0;
       else
         *pdpe = (uint64_t)pde;
     }
     *pdpe = *pdpe | PRESENT | WRITE;
     pde = (uint64_t *) (((uint64_t)PGDIR_ADDR + (pml4e_offset<<21) + (pdpe_offset <<12))+ pde_offset); 
     for(i=0;i<512;i++)       
       pde[i] = 0;

     if((*pde & PRESENT) == 0)
     {
       pte = pmmngr_alloc_block();
       if(pte ==0)
         return 0;
       else
         *pde = (uint64_t)pte;
     }
     *pde = *pde | WRITE | PRESENT;
     pte = (uint64_t*)((uint64_t)PGTBL_ADDR+ (pml4e_offset<<30) + (pdpe_offset<<21) + (pde_offset<<12) + pte_offset);  

     for(i=0;i<512;i++)
          pte[i] = 0;  

     return pte;
}
*/

/*
uint64_t* pte_t(uint64_t* addr)
{
     uint64_t pml4e_offset = (0x1ff&((uint64_t)addr>>39));
     uint64_t pdpe_offset  = (0x1ff&((uint64_t)addr>>30));
     uint64_t pde_offset   = (0x1ff&((uint64_t)addr>>21));
     uint64_t pte_offset   = (0x1ff&((uint64_t)addr>>12));
     uint64_t *pml4e = PML4T_ADDR + pml4e_offset;
     int i;
     uint64_t *pdpe;
     if(!(*pml4e & PRESENT)) 
     {
        if(!(*pml4e = (uint64_t)(pdpe = pmmngr_alloc_block()   )))
                return 0; // out of memory^M
        *pml4e |= PRESENT | WRITE;
         pdpe = (uint64_t*)((uint64_t)PDPT_ADDR+(pml4e_offset<<12));
        //Make all PDP entries 0^M
        for(i=0;i<512;i++) {
                            pdpe[i]=0;
                            //printf("%d %p\n",i,pdp[i]);^M
        }
     }
     pdpe = (uint64_t*)((uint64_t)PDPT_ADDR+(pml4e_offset<<12) + pdpe_offset);

     uint64_t *pde;
     if(!(*pdpe & PRESENT)) // allocate PDT if not present^M
     {
        //printf("allocating page for PDE");
        if(!(*pdpe = (uint64_t)(pde = pmmngr_alloc_block()  )))
                return 0; // out of memory
        *pdpe |= PRESENT | WRITE;
        pde = (uint64_t*)((uint64_t)PGDIR_ADDR+(pml4e_offset<<21)+(pdpe_offset<<12));
        // Make all PD entries 0
        for(i=0;i<512;i++) {
                            pde[i]=0;
        }
     }
     pde = (uint64_t*)((uint64_t)PGDIR_ADDR+(pml4e_offset<<21)+(pdpe_offset<<12) + pde_offset);
     uint64_t *pte;
     if(!(*pde & PRESENT)) // allocate PT if not present^M
     {
        if(!(*pde = (uint64_t)(pte = pmmngr_alloc_block() )))
                return 0; // out of memory^M
        *pde |= PRESENT | WRITE;
        pte = (uint64_t*)((uint64_t)PGTBL_ADDR+(pml4e_offset<<30)+(pdpe_offset<<21)+(pde_offset<<12));
         // Make all entries 0^M
        for(i=0;i<512;i++) {
                            pte[i]=0;
                            //printf("PT: %d %p\n",i,pt[i]);
        }
     }
     pte = (uint64_t*)((uint64_t)PGTBL_ADDR+(pml4e_offset<<30)+(pdpe_offset<<21)+(pde_offset<<12) + pte_offset);
     return pte;
}

*/


/*******************************
References References :  http://forum.osdev.org/viewtopic.php?f=1&t=22037
********************************/
uint64_t* Pte(uint64_t* addr)
{
     uint64_t *pte;
     pte = (uint64_t*)(Pt(addr))+(0x1ff&((uint64_t)addr>>12));
     return pte;
}


uint64_t* Pt(uint64_t* addr)
{
     int i;
     uint64_t *pde = Pde(addr);
     uint64_t *pt;
     if(!(*pde & PRESENT)) 
     {
        if(!(*pde = (uint64_t)(pt = pmmngr_alloc_block())))
                return 0; 
        *pde |= PRESENT | WRITE | USER;
        pt = (uint64_t*)((uint64_t)PGTBL_ADDR+((0x1ff&((uint64_t)addr>>39))<<30)+((0x1ff&((uint64_t)addr>>30))<<21)+((0x1ff&((uint64_t)addr>>21))<<12));
        for(i=0;i<512;i++) 
        {
           pt[i]=0;
        }
     }
     return (pt = (uint64_t*)((uint64_t)PGTBL_ADDR+((0x1ff&((uint64_t)addr>>39))<<30)+((0x1ff&((uint64_t)addr>>30))<<21)+((0x1ff&((uint64_t)addr>>21))<<12)));
}

uint64_t* Pde(uint64_t* addr)
{
     uint64_t *pde;
     return (pde = (uint64_t*)(Pd(addr))+(0x1ff&((uint64_t)addr>>21)));
}

uint64_t* Pd(uint64_t* addr)
{
     int i;
     uint64_t *pd;
     uint64_t *pdpe = Pdpe(addr);
     if(!(*pdpe & PRESENT)) 
     {
        if(!(*pdpe = (uint64_t)(pd = pmmngr_alloc_block())))
                return 0; 
        *pdpe |= PRESENT | WRITE | USER;
        pd = (uint64_t*)((uint64_t)PGDIR_ADDR+((0x1ff&((uint64_t)addr>>39))<<21)+((0x1ff&((uint64_t)addr>>30))<<12));
        for(i=0;i<512;i++) 
        {
           pd[i]=0;
        }
     }
     return (pd = (uint64_t*)((uint64_t)PGDIR_ADDR+((0x1ff&((uint64_t)addr>>39))<<21)+((0x1ff&((uint64_t)addr>>30))<<12)));
}

uint64_t* Pdpe(uint64_t* addr)
{
     uint64_t *pdpe;
     return (pdpe = (uint64_t*)(Pdp(addr))+ (0x1ff&((uint64_t)addr>>30)) );
}

uint64_t* Pdp(uint64_t* addr)
{
     int i;
     uint64_t *pdp, *pml4e;
     pml4e = Pml4e(addr);
     if(!(*pml4e & PRESENT)) // allocate PDPT 
     {
        if(!(*pml4e = (uint64_t)(pdp = pmmngr_alloc_block())))
                return 0; // 
        *pml4e |= PRESENT | WRITE |USER;
         pdp = (uint64_t*)((uint64_t)PDPT_ADDR+((0x1ff&((uint64_t)addr>>39))<<12));
        for(i=0;i<512;i++) 
        {
          pdp[i]=0;
        }
     }
     return (pdp = (uint64_t*)((uint64_t)PDPT_ADDR+((0x1ff&((uint64_t)addr>>39))<<12)));
}

uint64_t* Pml4e(uint64_t* addr)
{
     return ((uint64_t *)(PML4T_ADDR + (0x1ff&((uint64_t)addr>>39))));   
}



//assumption one page will be sufficient for each VMA
struct vm_area_struct *malloc_vma(struct mm_struct *mm)
{
  struct vm_area_struct *vm_tail;
  char *tmp;
  if(mm->mmap == NULL)  // first vma allocate one page for it
   {
     tmp=(char *)kmalloc(1); // this will allocate one page so just pass size>0
     vm_tail = (struct vm_area_struct *)tmp;
     mm->mmap = vm_tail;
     mm->count += 1;
     return (struct vm_area_struct *)tmp;
   }
  else
   {
     vm_tail = mm->mmap;
     while(vm_tail->vm_next != NULL) 
         vm_tail = vm_tail->vm_next;
     
     tmp = (char *)vm_tail + sizeof(struct vm_area_struct);  // go to the next vma in the same page (base +size)
     vm_tail->vm_next = (struct vm_area_struct *)tmp;
     mm->count += 1;
     return (struct vm_area_struct *)tmp;
   }
}




