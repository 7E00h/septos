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
FAT           equ 0x2000 ; 0x2000 - 0x4000: 8 sectors of the FAT
                         ; (accounts for 2048 clusters or 16k sectors or 8 MB)
ROOT_DIR      equ 0x4000 ; 0x4000 - 0x4200: First sector of the root directory
MEM_INFO      equ 0x4200 ; 0x4200 - 0x4400: Memory information structure from INT 0x15, EAX = 0xE820
                         ; This data is not parsed in the bootloader, but passed to kernel.
MEM_INFO_AMT  equ 0x4400 ; 0x4400 - 0x4002: Amount of entries in the above structure
PML4          equ 0x5000 ; Page
PDP           equ 0x6000 ; Page

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