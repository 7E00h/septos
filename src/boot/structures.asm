; --------------------
; -- structures.asm --
; --------------------
; 
; Contains data addresses for stage1.asm and stage2.asm
; Contains structure information for FAT32 and ELF
;

; ----------------
; -- Memory Map --
; ----------------
;
; https://wiki.osdev.org/Memory_Map_(x86)
;

DRIVE         equ 0x1000 ; 0x1000 - 0x1002: Drive # from BIOS (for int 0x13)
CLUSTER_START equ 0x1002 ; 0x1002 - 0x1006: Sector # of the first data cluster
CLUSTER_SIZE  equ 0x1006 ; 0x1006 - 0x1008: Size of each cluster (in sectors)
FAT           equ 0x2000 ; 0x2000 - 0x3000: 8 sectors of the FAT
                         ; (accounts for 1024 clusters or 8192 sectors or 4 MB)
ROOT_DIR      equ 0x3000 ; 0x3000 - 0x3200: First sector of the root directory
PML4          equ 0x4000 ; Page
PDP           equ 0x5000 ; Page

STAGE2        equ 0xA000 ; 0xA000 - 0xF000: STAGE2.BIN

ELF_BASE      equ 0x100000

; Header offsets
ELFe_entry     equ 0x18
ELFe_phoff     equ 0x20
ELFe_phentsize equ 0x36
ELFe_phnum     equ 0x38

; Segment entry offsets
ELFp_type   equ 0x00
ELFp_offset equ 0x08
ELFp_vaddr  equ 0x10
ELFp_filesz equ 0x20