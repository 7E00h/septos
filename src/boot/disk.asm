%include "src/boot/structures.asm"

BITS 16

DIRENT_NAME      equ 0  ; 8.3 file name
DIRENT_ATTR      equ 11 ; File attributes (0x0F for long entry)
DIRENT_CLUS_HIGH equ 20 ; High 16 bits of cluster #
DIRENT_CLUS_LOW  equ 26 ; Low 16 bits of cluster #

; ---------------
; -- disk_read --
; ---------------
; 
; Simply reads sectors from the disk
; -> Valid DAP
; <- N/A
;

disk_read:
	pushad
	mov ah, 0x42
	mov dl, [DRIVE]
	mov si, DAP
	int 0x13
	popad
	ret

; --------------------
; -- fat32_clus2sec --
; --------------------
;
; Converts a cluster # to sector # (in LBA)
; -> EAX: cluster #
; <- EAX: sector #
;

fat32_clus2sec:
	push ebx
	push edx

	mov ebx, [CLUSTER_START]

	sub eax, 2
	mul word [CLUSTER_SIZE]
	add eax, ebx

	pop edx
	pop ebx
	ret

; --------------------
; -- fat32_findfile --
; --------------------
;
; Locates a file in a directory
; -> SI: directory pointer
;    DI: file name pointer
; <- EDI: cluster # (0 for not found)
;

fat32_findfile:
.loop_entry:
	mov al, [si + DIRENT_NAME]

	; End of entry array
	cmp al, 0
	je .not_found

	; Unused entry
	cmp al, 0xE5
	je .next

	; Long entries not supported
	mov al, [si + DIRENT_ATTR]
	cmp al, 0x0F
	je .next

	; Compare names
	push di
	push si
	mov cx, 11
	repe cmpsb
	pop si
	pop di

	cmp cx, 0
	je .found

.next:
	add si, 32
	jmp .loop_entry

.found:
	xor edi, edi
	mov di, [si + DIRENT_CLUS_HIGH]
	shl edi, 16
	mov di, [si + DIRENT_CLUS_LOW]
	ret

.not_found:
	xor di, di
	ret


; --------------------
; -- fat32_readfile --
; --------------------
;
; Reads a file into memory
; -> EAX: cluster #
;    ES:DI: destination
; <- EAX: cluster value
;

fat32_readfile:
	mov cx, 16 ; Make sure it doesn't load more than 64k at a time

.read_cluster:
	; If done...
	cmp eax, 0x0FFFFFF8
	jge .done

	; Configure DAP
	mov word [DAP.Amount], 8
	mov [DAP.Segment], es
	mov [DAP.Offset], di

	; Read cluster into memory
	dec cx
	push eax
	call fat32_clus2sec
	mov [DAP.StartLow], eax

	call disk_read
	pop eax

	; Get next cluster value
	call fat32_readcluster

	; More than 64k?
	cmp cx, 0
	je .done

.next:
	; mov [DAP.Offset], di
	; add word [DAP.Offset], 0x1000
	add di, 0x1000

	jmp .read_cluster

.done:
	ret

; -----------------------
; -- fat32_readcluster --
; -----------------------
;
; Reads a cluster value from the FAT.
; Will dynamically load the appropriate FAT sector.
; This is slow and of course can be improved but >lazy.
; -> EAX: cluster index
; <- EAX: cluster value
;

fat32_readcluster:
	push ebx
	push edx
	push ecx
	push ds


	xor edx, edx
	mov ds, dx
	mov ebx, 128

	div ebx

	; Sector # is in EAX
	; Cluster # is in EDX
	shl edx, 2 ; * 4 because 4 bytes per entry

	; The first FAT comes right after reserved sectors.
	add ax, [FAT_SECTOR]

	; Load FAT sector
	mov word [DAP.Segment], 0
	mov word [DAP.Amount], 1
	mov word [DAP.Offset], FAT
	mov [DAP.StartLow], eax
	call disk_read

	mov eax, [FAT + edx]

	pop ds
	pop ecx
	pop edx
	pop ebx

	ret

DAP:
	.Size      db 16     ; Size of DAP
	.Zero      db 0      ; Zero
	.Amount    dw 1      ; # of sectors to read
	.Offset    dw BPB    ; Destination offset
	.Segment   dw 0x0000 ; Destination segment
	.StartLow  dd 0
	.StartHigh dd 0