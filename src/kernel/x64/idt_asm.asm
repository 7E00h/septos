BITS 64

global _asm_lidt
global _asm_sti
global _asm_cli

global _asm_default_handler

section .text
_asm_lidt:
    ; RDI contains address of table
    lidt [rdi]
    ret

_asm_sti:
    sti
    ret

_asm_cli:
    cli
    ret

align 4
_asm_default_handler:
    iretq