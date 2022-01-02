BITS 16
ORG  0xA000

%include "src/boot/structures.asm"

stage2:
    ; Retrieve memory information from BIOS
    call get_mem_info

    ; Locate KERNEL.ELF
    mov si, ROOT_DIR
    mov di, KERNEL_STR
    call fat32_findfile

    cmp edi, 0
    je no_kernel

    ; Put starting cluster in EAX
    ; DI contains starting cluster
    mov eax, edi
    mov bx, 0x1000
    mov es, bx

.load_loop:
    ; Load file starting at 0x10000
    ; The file will be loaded in 64k chunks
    ; Once a 64k chunk is read, copy it to 0x100000 (1 MB)
    xor di, di
    call fat32_readfile

    ; Copy to higher memory
    pushad
    push ds

    ; Some BIOS interrupts may overwrite DS thereby destroying
    ; the cached limit value. This is why it's called per
    ; loop.
    call enable_unreal

    mov esi, 0x10000 - 4
    mov edi, [DST]
    sub edi, 4
    mov ebx, 0x10000 / 4 ; MODIFIED ECX -> EBX

.loop:
    mov edx, ds:[esi + ebx*4]
    mov ds:[edi + ebx*4], edx
    dec ebx
    jnz .loop
.break:
    pop ds
    popad

    cmp eax, 0x0FFFFFF8
    jge .done

    add dword [DST], 0x10000
    jmp .load_loop

.done:
    ; Kernel is loaded at 1 MB now
    ; Time to swtich to long mode
    jmp mode_long

enable_unreal:
    push ds

    lgdt [GDT32.Pointer]

    mov eax, cr0
    or al, 1
    mov cr0, eax

    jmp $+2

    mov bx, GDT32.Data
    mov ds, bx

    and al, 0xFE
    mov cr0, eax
    
    pop ds
    ret

mode_long:
    ; -- Switch to long mode --
    ; Zero tables
    mov edi, PML4
    mov cr3, edi
    xor eax, eax
    mov ecx, 0x1000
    rep stosd
    mov edi, cr3

    ; Populate entries for 1 GB identity map
    mov word [edi], PDP + 3
    add edi, 0x1000
    mov word [edi],  (1 << 7) | 3

    ; Enable PAE
    mov eax, cr4
    or eax, 1 << 5
    mov cr4, eax

    ; Set long mode bit
    mov ecx, 0xC0000080
    rdmsr
    or eax, 1 << 8
    wrmsr

    ; Enable paging
    mov eax, cr0
    or eax, 1 << 31 | 1 << 0
    mov cr0, eax

    ; Switch :)
    lgdt [GDT64.Pointer]
    jmp GDT64.Code:long_mode
    hlt

; ------------------
; -- get_mem_info --
; ------------------
;
; See https://wiki.osdev.org/Detecting_Memory_(x86)#BIOS_Function:_INT_0x15.2C_EAX_.3D_0xE820
;

get_mem_info:
    pushad

    xor bp, bp

    xor ax, ax
    mov es, ax
    mov di, MEM_INFO
    xor ebx, ebx
    mov edx, 0x534D4150 ; Some magic number
.loop
    mov eax, 0xE820     ; Interrupt #
    mov ecx, 24         ; Bytes to read

    int 0x15

    jc .done

    cmp ebx, 0
    je .done

    cmp eax, 0x534D4150
    jne .done

    inc bp
    add di, 24
    jmp .loop

.done
    mov [MEM_INFO_AMT], bp
    popad
    ret

no_kernel:
    hlt
    hlt
    hlt

BITS 64
long_mode:
    cli
    mov ax, GDT64.Data
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
   
    ; Now the ELF needs to be loaded
    ; This should be easy since the CPU is in long mode

load_elf:
    xor rdx, rdx
    mov rbp, ELF_BASE
    mov bx,  [rbp+ELFe_phnum]     ; # of segments in program header
    mov dx,  [rbp+ELFe_phentsize] ; Size of segment entry
    mov r15, [rbp+ELFe_entry]     ; Entry point
    add rbp, [rbp+ELFe_phoff]     ; # Offset of program header

.load_segment:
    cmp bx, 0
    jz .done

    ; Check type
    mov eax, [rbp+ELFp_type]
    cmp eax, 1
    jne .next

    ; Load segment
    mov rdi, [rbp+ELFp_vaddr]
    mov rsi, [rbp+ELFp_offset]
    add rsi, ELF_BASE
    mov rcx, [rbp+ELFp_filesz]

    rep movsb

.next:
    add rbp, rdx
    dec bx
    jmp .load_segment

.done:
    ; Pass in MEM_INFO
    mov rdi, MEM_INFO
    xor rsi, rsi
    mov si, [MEM_INFO_AMT]
    call r15
    hlt

GDT32:
    .Null: equ $ - GDT32
    dq 0
    .Data: equ $ - GDT32
    dw 0xFFFF     ; Limit 0:15
    dw 0x0000     ; Base 0:15
    db 0x00       ; Base 16:23
    db 0b10010010 ; Access Byte
    db 0b01001111 ; Flags & Limit 16:19
    db 0x00       ; Base 24:31
    .Pointer:
    dw $ - GDT32 - 1
    dd GDT32

GDT64:                           ; Global Descriptor Table (64-bit).
    .Null: equ $ - GDT64         ; The null descriptor.
    dw 0xFFFF                    ; Limit (low).
    dw 0                         ; Base (low).
    db 0                         ; Base (middle)
    db 0                         ; Access.
    db 1                         ; Granularity.
    db 0                         ; Base (high).
    .Code: equ $ - GDT64         ; The code descriptor.
    dw 0                         ; Limit (low).
    dw 0                         ; Base (low).
    db 0                         ; Base (middle)
    db 10011010b                 ; Access (exec/read).
    db 10101111b                 ; Granularity, 64 bits flag, limit19:16.
    db 0                         ; Base (high).
    .Data: equ $ - GDT64         ; The data descriptor.
    dw 0                         ; Limit (low).
    dw 0                         ; Base (low).
    db 0                         ; Base (middle)
    db 10010010b                 ; Access (read/write).
    db 00000000b                 ; Granularity.
    db 0                         ; Base (high).
    .Pointer:                    ; The GDT-pointer.
    dw $ - GDT64 - 1             ; Limit.
    dq GDT64                     ; Base.

SRC: dd 0x10000
DST: dd 0x100000

KERNEL_STR: db "KERNEL  ELF"

%include "src/boot/disk.asm"