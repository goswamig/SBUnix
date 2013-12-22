# Author: cds
# Copyright ©2013, by Gautam Kumar, Haseeb Niaz, Krishna Kanhaiya.

.text

######
# load a new IDT
#  parameter 1: address of idtr
.global _x86_64_asm_igdt
_x86_64_asm_igdt:
    lidt (%rdi)
    retq                       # far-return to new cs descriptor ( the retq below )

