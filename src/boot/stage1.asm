BITS 16
ORG  0x7C00

%include "src/boot/structures.asm"

BPB:
    jmp short stage1
    nop
    
    .OEM:               times 8  db 0
	.BytesPerSector:    times 2  db 0
	.SectorsPerCluster: times 1  db 0
	.ReservedSectors:   times 2  db 0
	.NumberOfFATs:      times 1  db 0
	.NumberOfDirEnts:   times 2  db 0
	.SectorsInVolume:   times 2  db 0
	.MediaDescType:     times 1  db 0
	.SectorsPerFAT:     times 2  db 0
	.SectorsPerTrack:   times 2  db 0
	.NumberOfHeads:     times 2  db 0
	.HiddenSectors:     times 4  db 0
	.SectorCountLarge:  times 4  db 0

	.SectorsPerFAT32:   times 4  db 0 ; TODO fix misleading name!
	.Flags:             times 2  db 0
	.FATVersion:        times 2  db 0
	.RootDirCluster:    times 4  db 0
	.FSInfoSector:      times 2  db 0
	.BackupBootSector:  times 2  db 0
	.Reserved:          times 12 db 0
	.DriveNumber:       times 1  db 0
	.FlagsNT:           times 1  db 0
	.Signature:         times 1  db 0
	.VIDNumber:         times 4  db 0
	.VolumeLabelStr:    times 11 db 0
	.SysIDStr:          times 8  db 0

stage1:
    cli
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    jmp 0x0000:.cs_jmp

.cs_jmp:
    mov sp, 0xF000
    mov [DRIVE], dl

	mov al, [BPB.SectorsPerCluster]
	mov [CLUSTER_SIZE], ax

	; Load FAT
	; The first FAT is immediately after the reserved sectors
	mov bx, [BPB.ReservedSectors]
	mov [DAP.StartLow], ebx
	call disk_read

	; Calculate first data cluster offset
	; start = reserved + FATs * size
	; BX contains reserved sectors
	xor dx, dx
	mov dl,  [BPB.NumberOfFATs]
	mov eax, [BPB.SectorsPerFAT32]
	mul dx
	add eax, ebx
	mov [CLUSTER_START], eax

	; Read root directory into memory
	mov word  [DAP.Amount], 1
	mov word  [DAP.Offset], ROOT_DIR
	mov dword [DAP.StartLow],  eax
	call disk_read

	; Find STAGE2.BIN
	mov si, ROOT_DIR
	mov di, STAGE2_STR
	call fat32_findfile

	cmp di, 0
	je .no_stage2

	; Read file into memory
	; ES:DI = destination
	; EAX   = cluster
	movsx eax, di
	mov di, 0xA000
	call fat32_readfile

	; STAGE2.BIN is now loaded, jump to it
	jmp 0xA000

.no_stage2:
	hlt

%include "src/boot/disk.asm"

STAGE2_STR: db "STAGE2  BIN"

times 510 - ($ - $$) db 0
dw 0xAA55