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
CLUSTER_SIZE  equ 0x1006 ; 0x1006 - 0x1008: Size of each cluster (in sectors)
FAT_SECTOR    equ 0x1008 ; 0x1008 - 0x100A: Beginning sector of the first FAT
PART_OFFSET   equ 0x1100 ; Offset of the FAT32 partition
BPB           equ 0x1200 ; BPB
CLUSTER_START equ 0x1220 ; Sector # of the first data cluster
FAT           equ 0x2000 ; 0x2000 - 0x5000: 24 sectors of the FAT
                         ; (accounts for 3072 clusters or 24576 sectors or 12 MB)
ROOT_DIR      equ 0x5000 ; 0x5000 - 0x5200: First sector of the root directory
MEM_INFO      equ 0x5200 ; 0x5200 - 0x5400: Memory information structure from INT 0x15, EAX = 0xE820
                         ; This data is not parsed in the bootloader, but passed to kernel.
MEM_INFO_AMT  equ 0x5400 ; 0x5400 - 0x5002: Amount of entries in the above structure
PML4          equ 0x6000 ; Page
PDP           equ 0x7000 ; Page

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
    
BPB_OEM:               equ 0x03
BPB_BytesPerSector:    equ 0x0B
BPB_SectorsPerCluster: equ 0x0D
BPB_ReservedSectors:   equ 0x0E
BPB_NumberOfFATs:      equ 0x10
BPB_NumberOfDirEnts:   equ 0x11
BPB_SectorsInVolume:   equ 0x13
BPB_MediaDescType:     equ 0x15
BPB_SectorsPerFAT:     equ 0x16
BPB_SectorsPerTrack:   equ 0x18
BPB_NumberOfHeads:     equ 0x1A
BPB_HiddenSectors:     equ 0x1C
BPB_SectorCountLarge:  equ 0x20

BPB_SectorsPerFAT32:   equ 0x24 ; TODO fix misleading name!
BPB_Flags:             equ 0x28
BPB_FATVersion:        equ 0x2A
BPB_RootDirCluster:    equ 0x2C
BPB_FSInfoSector:      equ 0x30
BPB_BackupBootSector:  equ 0x32
BPB_Reserved:          equ 0x30
BPB_DriveNumber:       equ 0x40
BPB_FlagsNT:           equ 0x41
BPB_Signature:         equ 0x42
BPB_VIDNumber:         equ 0x43
BPB_VolumeLabelStr:    equ 0x40
BPB_SysIDStr:          equ 0x52