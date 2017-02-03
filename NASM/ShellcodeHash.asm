[BITS 32]

SECTION .data
msg:	db __utf16__("Hash for %s is: %X"),0,0
	global _main		; make label available to linker 
	extern _wprintf
SECTION .text
_main:
	mov eax, [fs:0x30]			; get PEB
	mov eax, [eax + 0x10]       ; get RTL_USER_PROCESS_PARAMETERS
	add eax, 0x44				; 0x40 to _UNICODE_STRING + 0x02 to StrPtr
	push dword [eax]
	movzx ecx, word [eax - 0x04]; cmdline length

	cld					        ; skip cmdline to first space
	pop edi
	mov eax, 0x20
	repne scasw				    ; skip nonspaces
	repe scasw				    ; skip spaces
	sub edi, 2				    ; backtrace a word
	push edi					; cmdline text

	mov esi, edi
	xor edi, edi
loop_modname:
    lodsw                       ; Read in the next word of the unicode cmdline parameter
	cmp al, 0
	jz computed
	cmp al, '!'
	jz loop_modname             ; Ignore ! character
    cmp al, 'a'                 ; Some versions of Windows use lower case module names
    jl not_lowercase
    sub al, 0x20                ; If so normalise to uppercase
not_lowercase:
    ror edi, 13                 ; Rotate right our hash value
    add edi, eax                ; Add the next byte of the name
    jmp loop_modname            ; Loop untill we have read enough	

computed:		
	pop eax                     ; cmdline text
	push edi					; computed hash
	push eax
	push msg
	call _wprintf
	add esp, 0x0c
	ret
	


