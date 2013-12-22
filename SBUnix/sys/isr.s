.text
# Copyright © 2013, by Gautam Kumar, Haseeb Niaz, Krishna Kanhaiya
######
# load a new IDT
#  parameter 1: address of gdtr
#  parameter 2: new code descriptor offset
#  parameter 3: new data descriptor offset

##.macro set_interrupt index
##.global x86_64_isr_vector\index
#.global x86_64_isr_vector0
#x86_64_isr_vector0:
#            cli
#            pushq %rax
#            pushq %rcx
#            pushq %rdx
#            pushq %rsi
#            pushq %rdi
#            pushq %r8
#            pushq %r9
#            pushq %r10
#            pushq %r11
#            movq  %rsp,%rdi
#            addq  $72, %rdi
#            call x86_64_handle_isr_vector0
#            popq %r11
#            popq %r10
#            popq %r9
#            popq %r8
#            popq %rdi
#            popq %rsi
#            popq %rdx
#            popq %rcx
#            popq %rax
#            sti 
#            iretq





.global x86_64_isr_vector0
x86_64_isr_vector0:
     #   cli
        pushq %rax 
	pushq %rbx        # preserve
	pushq %rcx
	pushq %rdx
	pushq %rsi
	pushq %rdi
	pushq %rbp        # preserve
	pushq %r8
	pushq %r9
	pushq %r10
	pushq %r11
	pushq %r12        # preserve
	pushq %r13        # preserve
	pushq %r14        # preserve
	pushq %r15        # preserve

	call x86_64_handle_isr_vector0

	popq %r15        # preserve
	popq %r14        # preserve
	popq %r13        # preserve
	popq %r12        # preserve
	popq %r11
	popq %r10
	popq %r9
	popq %r8
	popq %rbp        # preserve
	popq %rdi
	popq %rsi
	popq %rdx
	popq %rcx
	popq %rbx        # preserve
        popq %rax 
     #   sti
	iretq


##########################  TIMER INERRUPT ###################
.global x86_64_isr_vector1
x86_64_isr_vector1:
#   cli
   pushq %rax
   pushq %rbx
   pushq %rcx
   pushq %rdx
   pushq %rsi
   pushq %rdi
   pushq %rbp
   pushq %r8
   pushq %r9
   pushq %r10
   pushq %r11
   pushq %r12
   pushq %r13
   pushq %r14
   pushq %r15
  
   call x86_64_handle_isr_vector1
   call schedule
  
   popq %r15
   popq %r14
   popq %r13
   popq %r12
   popq %r11
   popq %r10
   popq %r9
   popq %r8
   popq %rbp
   popq %rdi
   popq %rsi
   popq %rdx
   popq %rcx
   popq %rbx
   popq %rax   
#   sti
   iretq



##################### KEY BOARD ########################### 
.global x86_64_isr_vector2
x86_64_isr_vector2:
#   cli
   pushq %rax
   pushq %rbx
   pushq %rcx
   pushq %rdx
   pushq %rsi
   pushq %rdi
   pushq %rbp
   pushq %r8
   pushq %r9
   pushq %r10
   pushq %r11
   pushq %r12
   pushq %r13
   pushq %r14
   pushq %r15
  
   call x86_64_handle_isr_vector2

   popq %r15
   popq %r14
   popq %r13
   popq %r12
   popq %r11
   popq %r10
   popq %r9
   popq %r8
   popq %rbp
   popq %rdi
   popq %rsi
   popq %rdx
   popq %rcx
   popq %rbx
   popq %rax 
#   sti
   iretq



######### GPF ###############
.global x86_64_isr_vector13  
x86_64_isr_vector13:
#      cli
      pushq %rax
      pushq %rcx
      pushq %rdx
      pushq %rsi  
      pushq %rdi
      pushq %r8
      pushq %r9
      pushq %r10
      pushq %r11
    
      call x86_64_handle_isr_vector13

      popq %r11
      popq %r10
      popq %r9
      popq %r8
      popq %rdi
      popq %rsi 
      popq %rdx
      popq %rcx
      popq %rax 
#      sti
      iretq


######### Page Fault ###############
.global x86_64_isr_vector14
x86_64_isr_vector14:
#      cli
      pushq %rax
      pushq %rcx
      pushq %rdx
      pushq %rsi
      pushq %rdi
      pushq %r8
      pushq %r9
      pushq %r10
      pushq %r11

      call x86_64_handle_isr_vector14

      popq %r11
      popq %r10
      popq %r9
      popq %r8
      popq %rdi
      popq %rsi
      popq %rdx
      popq %rcx
      popq %rax   
      addq $0x8, %rsp
#      sti
      iretq




############################################## software interrupt begins #######################################
############################################## software interrupt begins #######################################
################################# NEVER PUSH %rax here , %rax has return value ################################# 
.global x86_64_isr_vector3  
x86_64_isr_vector3:
#   cli
   pushq %rbx
   pushq %rcx
   pushq %rdx
   pushq %rsi
   pushq %rdi
   pushq %rbp
   pushq %r8
   pushq %r9
   pushq %r10
   pushq %r11
   pushq %r12
   pushq %r13
   pushq %r14
   pushq %r15

   call x86_64_handle_isr_vector3

   popq %r15
   popq %r14
   popq %r13
   popq %r12
   popq %r11
   popq %r10
   popq %r9
   popq %r8
   popq %rbp
   popq %rdi
   popq %rsi
   popq %rdx
   popq %rcx
   popq %rbx
#   sti
   iretq

################################### yield for read ###############################  
.global x86_64_isr_vector4    
x86_64_isr_vector4:
#   cli
   pushq %rax
   pushq %rbx
   pushq %rcx
   pushq %rdx
   pushq %rsi
   pushq %rdi
   pushq %rbp
   pushq %r8
   pushq %r9
   pushq %r10
   pushq %r11
   pushq %r12
   pushq %r13
   pushq %r14
   pushq %r15

   call schedule 

   popq %r15
   popq %r14
   popq %r13
   popq %r12
   popq %r11
   popq %r10
   popq %r9
   popq %r8
   popq %rbp
   popq %rdi
   popq %rsi
   popq %rdx
   popq %rcx
   popq %rbx
   popq %rax
#   sti
   iretq 

#.endm

#set_interrupt 0
