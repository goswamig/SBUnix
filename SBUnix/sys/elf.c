#include <defs.h>
#include <system.h>
#include<sys/tarfs.h>
tarfs_entry tarfs_fs[100];

void* search (char *fname)  // search the file into tarfs dir, return the tarfs pointer
{

  struct posix_header_ustar *ps;
  char *ch = &_binary_tarfs_start;
  uint32_t size_of_posix_header = sizeof(struct posix_header_ustar);
  uint32_t ps_size;
  while(ch < &_binary_tarfs_end)
  {
    ps = (struct posix_header_ustar *)ch;
    ps_size = oct2dec(atoi(ps->size));
    if(strcmp(ps->name,fname)==0)
      return (void *)ch;
    if(ps_size%512) //512 alignment   
     ps_size = ((ps_size+512)/512)*512;
    ch += size_of_posix_header + ps_size;
  }
  return NULL;
}

/*
struct task_struct* elf_parse(char *fname) // map the elf entries
{
 int i;
 struct posix_header_ustar *file;
 file = (struct posix_header_ustar *)search(fname);
 file = file + 1; // point to the elf header
 struct ELF *elf =(struct ELF *)file;
 //now i have elf header lets play with it 
 
 struct task_struct* pcb = (struct task_struct *)malloc_pcb((uint64_t *)elf->e_entry);
 printf("%d pcb %p\n",pcb->pid,pcb);
 printf("entry %p\n",(uint64_t *)elf->e_entry);  
//lets get the section header from elfheader by using the offset of elfheader
 struct ELF_ph * elfph = (struct ELF_ph *)((uint64_t)elf + elf->e_phoff);

 for(i=0;i<elf->e_phnum;i++) { 
  if(elfph->p_type ==  PT_LOAD)
   {
  //   printf("We need to load this segment %d %d %d\n",elfph->p_vaddr,elfph->p_memsz,elfph->p_offset);   
    uint64_t* pte = Pte((uint64_t *)elfph->p_vaddr);
     if((*pte & PRESENT) == 0) // if frame is not allocated in pm
     {
       uint64_t* frame = pmmngr_alloc_block();
       if(frame == 0)
        {
          printf("I can't allocate here good bye\n");         
          return 0;   // frame allocation failed
       } 
        *pte =(uint64_t)frame | PRESENT | WRITE; //update into page table
     }
     int j;
     printf("Loading Started...%p %p\n",(uint64_t *)elfph->p_vaddr,elfph->p_memsz); 
     for(j=0; j<elfph->p_memsz; j++)  
     { 
      *((uint64_t *)(elfph->p_vaddr + j)) = *((uint64_t *)((uint64_t)elf + elfph->p_offset + j));  

   //   if((elfph->p_vaddr + j) ==  elf->e_entry  ) 
   //     printf("%c %c \n", *((uint64_t *)(elfph->p_vaddr +j)),*((uint64_t *)((uint64_t)elf + elfph->p_offset + j))); 
 }

 
// for(j= 0; j<(elfph->p_vaddr + elfph->p_memsz - elf->e_entry - 150); j++)
//     {
//      printf("%c %c \n", *((uint64_t *)(elf->e_entry+j)),*((uint64_t *)((uint64_t)elf + elfph->p_offset + 0xb0 + j)));
//     }

   printf("Loading Complete !!! %p\n",(uint64_t *)(elfph->p_vaddr + j)); 
   //  while(1); 
    //put this on ready _queue   
     ready_queue[pcb->pid] =(uint64_t)pcb;
     printf("%p %p %d\n",pcb,ready_queue[pcb->pid],pcb->pid); 
 // map_vma(elf,elfph,pcb);      
   }
 elfph= elfph + 1;
 }
 return pcb; 
 //return NULL;
}
*/

/*
  we have the mm for pcb we need to create the vma and map iT to mm of given pcb 
*/
void map_vma(struct ELF *elf, struct ELF_ph *elfph, struct task_struct *pcb)
{

} 


void tarfs_init()
{
       	struct posix_header_ustar *tarfs_var = (struct posix_header_ustar *)&_binary_tarfs_start;
	int i = 0, temp = 0;
	uint64_t size;
       	tarfs_entry tarfs_e;
	while(true)
	{
		tarfs_var = (struct posix_header_ustar *)(&_binary_tarfs_start + temp);
           	if(strlen(tarfs_var->name) == 0)
                	break;
          
		size = oct2dec(stoi(tarfs_var->size));
                
           	strcpy(&tarfs_e.name[0], tarfs_var->name);
           	tarfs_e.size = size;
           	tarfs_e.addr_hdr = (uint64_t)&_binary_tarfs_start + temp;
           	tarfs_e.typeflag = stoi(tarfs_var->typeflag);
           //     printf("%p %s..",tarfs_e.addr_hdr,tarfs_e.name);           
   	if(tarfs_e.typeflag == FILE_TYPE)
                	tarfs_e.par_ind = get_per_ind_file(&(tarfs_e.name[0]));
           	else if(tarfs_e.typeflag == DIRECTORY)
           	{
                	tarfs_e.par_ind = get_per_ind(&(tarfs_e.name[0]));
               		// curr_dir = i;
           	}
           
            
           	tarfs_fs[i] = tarfs_e;
           	i++;
		if(size == 0)
			temp = temp + 512;
		else
			temp +=  (size%512==0) ? size + 512 : size + (512 - size%512) + 512;
	  }
}
uint64_t read_dir(uint64_t addr)                                                                                                              
{
//	printf("%p\n",addr);   
	tarfs_entry tarfs_e;
	char *dir;
	int i = 0, parent = -1, len = 0;
	while(true )
	{  
		tarfs_e = tarfs_fs[i];
		if(strlen(tarfs_e.name) == 0)
			break;

		if( tarfs_e.addr_hdr == addr)
		{  
			parent = i;
			i++;
			dir = tarfs_e.name;
			len = strlen(dir);
	//		printf("%s %d\n",dir,len); 
			continue;
		}  
		if((strncmp(&(tarfs_e.name[0]), dir, strlen(dir)) == 0 ) &&( tarfs_e.par_ind == parent))
		{  
			printf("%s", tarfs_e.name + len);
			printf("\n");
		}  
		i++;
	}  
	return 0;
}





uint64_t open(char * file)
{
    	tarfs_entry tarfs_e;
    	int i = 0;
    	while(true )
    	{   
        	tarfs_e = tarfs_fs[i];
        	i++;
        	if(strlen(tarfs_e.name) == 0)
            		break;
    
        	if(strcmp(&(tarfs_e.name[0]), file) == 0 && tarfs_e.typeflag == FILE_TYPE)
            		return tarfs_e.addr_hdr;
    	}   
    	printf("\n No such file ");
    	printf("%s", file);
    	return 0;
}

uint64_t read_file(uint64_t file_addr, int size, uint64_t buf)
{
        
  	struct posix_header_ustar* file_hdr = (struct posix_header_ustar *) file_addr; 
    	int file_size =  oct2dec(stoi(file_hdr->size));
 	if(file_size < size)
        	size = file_size;
    	char* tmp =(char *)buf;
    	char* file_start_addr = (char *)(file_addr + 512);
    	int i = 0;
    	while(size-- > 0)
    	{
        	tmp[i++] = *file_start_addr++;
    	}    
    	tmp[i]='\0';
    	printf("\n");
    	printf("%s", tmp);
  	return file_size;
}

int get_per_ind(char* dir)
{
	char name[100];
    	int len = strlen(dir);
    	strcpy(&name[0], dir);
    	len = len-2;
   	// printf("  {%d} ", len); 
    	while(name[len] != '/')
    	{
        	len--;
        	if(len == 0)
            		return 999;
    	}
   	// printf("  {%d} ", len); 
    	name[++len] = '\0';
    	int i = 0;
    	while(strcmp(&name[0], &(tarfs_fs[i].name[0])) !=  0)
        	i++;
    	// printf("parent {%d}", i);
    	return i;
}

int get_per_ind_file(char* dir)
{
    	char name[100];
    	int len = strlen(dir);
    	strcpy(&name[0], dir);
    	len = len-1;
   	// printf("  {%d} ", len); 
    	while(name[len] != '/')
    	{
        	len--;
        	if(len == 0)
            		return 999;
    	}
   	// printf("  {%d} ", len); 
    	name[++len] = '\0';
    	int i = 0;
    	while(strcmp(&name[0], &(tarfs_fs[i].name[0])) !=  0)
        	i++;
    	// printf("parent {%d}", i);
    	return i;
}

uint64_t is_file_exists(char* filename)
{
	// printf("\n Binary tarfs start:  %x", &_binary_tarfs_start);
	struct posix_header_ustar* test_tarfs = (struct posix_header_ustar *)&_binary_tarfs_start;
	int i = 1, temp = 512;
	uint64_t size;
	// printf("\n Name %s \t Mode %s \t uid %s \t gid %s\t",test_tarfs->name, test_tarfs->mode, test_tarfs->uid, test_tarfs->gid);
	while(strlen(test_tarfs->name) != 0)
	{
		test_tarfs = (struct posix_header_ustar *)(&_binary_tarfs_start + temp);
		size = oct2dec(stoi(test_tarfs->size));
		// printf("\nName of file is %s and size in octal is %d", test_tarfs->name, test_tarfs->size);
		if(strlen(test_tarfs->name) == 0)
			return 999;
		if(strcmp(test_tarfs->name, filename) >= 0)
			return temp + 512;
		if(size == 0)
			temp = temp + 512;
		else
			temp +=  (size%512==0) ? size + 512 : size + (512 - size%512) + 512;
		// printf("    %d", temp);
		i += 1;
	}
	// printf("%d", size);
	return 0;
}



uint64_t open_dir(char * dir)

{
      //  printf("%s...\n",dir);   
    	tarfs_entry tarfs_e;
    	int i = 0;
    	while(true )
    	{
        	tarfs_e = tarfs_fs[i];
        	i++;
        	if(strlen(tarfs_e.name) == 0)
            		break;
        
        	if(strcmp(&(tarfs_e.name[0]), dir) == 0 && tarfs_e.typeflag == DIRECTORY)
                		return tarfs_e.addr_hdr;    
    	}
    	printf("\n No such directory ");
    	printf("%s", dir);
    	return 0;
}

uint64_t closedir(char *dir)
{
  return 0;
}










