BITS 64

global _asm_out_8
global _asm_out_16
global _asm_out_32

global _asm_in_8
global _asm_in_16
global _asm_in_32

section .text

_asm_out_8:
    push rax
    push rdx
    movsx rdx, edi ; Address
    movsx rax, esi ; Data
    out dx, al
    pop rdx
    pop rax
    ret

_asm_out_16:
    push rax
    push rdx
    movsx rdx, edi ; Address
    movsx rax, esi ; Data
    out dx, ax
    pop rdx
    pop rax
    ret

_asm_out_32:
    push rax
    push rdx
    movsx rdx, edi ; Address
    movsx rax, esi ; Data
    out dx, eax
    pop rdx
    pop rax
    ret

_asm_in_8:
    push rdx
    xor rax, rax
    movsx rdx, edi ; Address
    in al, dx
    pop rdx
    ret

_asm_in_16:
    push rdx
    xor rax, rax
    movsx rdx, edi ; Address
    in ax, dx
    pop rdx
    ret

_asm_in_32:
    push rdx
    xor rax, rax
    movsx rdx, edi ; Address
    in eax, dx
    pop rdx
    ret