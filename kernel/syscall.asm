
;-------------------------------------------------------
;						syscall.asm
;-------------------------------------------------------

INT_VECTOR_SYS_CALL equ 0x80
_NR_sendrec equ 0

global sendrec

bits 32
[section .text]

;-----------------------------------------------------------
; PUBLIC int sendrec(int function, int src_dest, MESSAGE *m)
;-----------------------------------------------------------
sendrec:
	mov eax, _NR_sendrec
	mov ebx, [esp + 4]  ; function
	mov ecx, [esp + 8]  ; src_dest
	mov edx, [esp + 12] ; m (指向消息的指针)
	int INT_VECTOR_SYS_CALL
	ret
