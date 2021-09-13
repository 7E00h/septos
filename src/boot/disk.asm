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
	pusha
	mov ah, 0x42
	mov dl, [DRIVE]
	mov si, DAP
	int 0x13
	popa
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
	sub eax, 2
	mul word [CLUSTER_SIZE]
	add ax, [CLUSTER_START]
	ret

; --------------------
; -- fat32_findfile --
; --------------------
;
; Locates a file in a directory
; -> SI: directory pointer
;    DI: file name pointer
; <- DI: cluster # (0 for not found)
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
; -> AX: cluster #
;    ES:DI: destination
; <- EAX: cluster value
;

fat32_readfile:
	mov word [DAP.Amount], 8
	mov [DAP.Segment], es
	mov [DAP.Offset], di
	mov cx, 16 ; Make sure it doesn't load more than 64k at a time

.read_cluster:
	; If done...
	cmp eax, 0x0FFFFFF8
	jge .done

	; Read cluster into memory
	dec cx
	push eax
	call fat32_clus2sec
	mov [DAP.StartLow], eax
	call disk_read

	; Get next cluster
	pop eax
	shl eax, 2
	mov eax, [FAT + eax]

	; More than 64k?
	cmp cx, 0
	je .done

.next:
	add word [DAP.Offset], 0x1000
	jmp .read_cluster

.done:
	ret

DAP:
	.Size      db 16     ; Size of DAP
	.Zero      db 0      ; Zero
	.Amount    dw 16     ; # of sectors to read
	.Offset    dw FAT    ; Destination offset
	.Segment   dw 0x0000 ; Destination segment
	.StartLow  dd 0
	.StartHigh dd 0