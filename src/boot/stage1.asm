BITS 16
ORG  0x7C00

%include "src/boot/structures.asm"

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
	mov sp, 0x9000
	mov [DRIVE], dl

	; Enable A20
	in al, 0x92
	or al, 2
	out 0x92, al

	; Store the FAT32 partition offset
	mov eax, [Entry.LBAFirst]
	mov [PART_OFFSET], eax

	; Load the BPB (first sector at offset)
	mov [DAP.StartLow], eax
	call disk_read

	; Store cluster size
	xor ax, ax
	mov al, [BPB + BPB_SectorsPerCluster]
	mov [CLUSTER_SIZE], ax

	; mov word [DAP.Offset], FAT
	; mov word [DAP.Amount], 24

	; Load FAT
	; The first FAT is immediately after the reserved sectors
	mov eax, [PART_OFFSET]

	mov bx, [BPB + BPB_ReservedSectors]
	add ebx, eax ; add partition offset
	mov [FAT_SECTOR], bx
	; mov [DAP.StartLow], ebx
	; call disk_read

	; Calculate first data cluster offset
	; start = reserved + FATs * size
	; BX contains reserved sectors
	xor dx, dx
	mov dl,  [BPB + BPB_NumberOfFATs]
	mov eax, [BPB + BPB_SectorsPerFAT32]
	mul dx
	add eax, ebx
	mov [CLUSTER_START], eax

	; Read root directory into memory
	mov eax, [BPB + BPB_RootDirCluster]
	call fat32_clus2sec

	mov word  [DAP.Amount], 1
	mov word  [DAP.Offset], ROOT_DIR
	mov dword [DAP.StartLow],  eax
	call disk_read

	; xor di, di

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
	mov ax, 0x6969
	hlt

%include "src/boot/disk.asm"

STAGE2_STR: db "STAGE2  BIN"

times 446 - ($ - $$) db 0

Entry:
	.Status:       db 0
	.StartHead     db 0
	.StartSector   db 0
	.StartCylinder db 0
	.Type          db 0
	.EndHead       db 0
	.EndSector     db 0
	.EndCylinder   db 0
	.LBAFirst      dd 0
	.PartSize      dd 0

; Zero out the loser partitions
times 48 db 0