[BITS 32]
safeseh __except_handler3	; this is obligatory

section .drectve info
db "/defaultlib:msvcrt.lib"

section .data
extern _puts
extern _printf
extern _strcpy
extern __except_handler3

; sehtestfunct stack map
;_excCode        : dd -28h
;buf             : db -24h
;_saved_esp      : dd -18h
;_exception_info : dd -14h
;_next           : dd -10h
;_handler        : dd -0Ch
;_scopetable     : dd -8
;_trylevel       : dd -4
;str             : dd  8
; end stack map

section .text
sehtestfunct:
  push    ebp
  mov     ebp, esp
  push    -1
  push    _func1_scopetable
  push    __except_handler3
  mov     eax, [fs:0]
  push    eax
  mov     [fs:0], esp 
  add     esp, -0x18
  push    ebx
  push    esi
  push    edi
  ; --- end of prolog ---

  mov [ebp-0x18], esp            ; save stack pointer for unwind
  mov     dword [ebp-4], 0       ; try..finally entry
  mov     dword [ebp-4], 1       ; try..except entry
  mov     dword [0], 0xffffffff  ; JEB!
  mov     dword [ebp-4], 0       ; try..except exit if filter returns CONTINUE_EXECUTION
  jmp     short _endoftry1

_func1_filter1:                  ; __except() filter of try block 1
  mov     ecx, [ebp-0x14]        ; ecx points to EXCEPTION_POINTERS
  mov     edx, [ecx]             ; [ecx+EXCEPTION_POINTERS.ExceptionRecord]
  mov     eax, [edx]             ; [edx+EXCEPTION_RECORD.ExceptionCode]
  mov     [ebp-0x28], eax        ; save ExceptionCode on the stack
  mov     ecx, [ebp-0x28]        ; get ExceptionCode
  xor     eax, eax               ; clear eax 
  cmp     ecx, 0xC0000005        ; EXCEPTION_ACCESS_VIOLATION
  setz    al                     ; set eax if ExceptionCode == EXCEPTION_ACCESS_VIOLATION
  retn

_func1_handler1:                 ; beginning of handler for try block 1
  mov     esp, [ebp-0x18]        ; restore esp (unwind)
  push    aAccessViolatio        ; "Access violation"
  call    _printf
  add     esp, 4                 ; _cdecl stack cleanup
  mov     dword [ebp-4], 0       ; try..except exit

_endoftry1:
  mov     edx, [ebp+8]           ; get argument
  push    edx                    ; ..on the stack
  lea     eax, [ebp-0x24]        ; get buffer address
  push    eax                    ; ..on the stacl
  call    _strcpy
  add     esp, 8                 ; try..except exit
  mov     dword [ebp-4], -1      ; try..except
  call    _func1_handler0        ; execute __finally of try block 0
  jmp     short _endoftry0       ; after handling jump to cleanup code

_func1_handler0:                 ; __finally handler of try block 0
  push    aInFinally ; "in finally"
  call    _puts
  add     esp, 4
  retn

_endoftry0:
  ; --- epilog ---
  mov     ecx, [ebp-0x10]        ; get stack stored pointer to next _EXCEPTION_REGISTRATION_RECORD
  mov     [fs:0], ecx            ; restore orginal exception handling
  pop     edi
  pop     esi
  pop     ebx
  mov     esp, ebp
  pop     ebp
  retn


global _main
_main:
  push aInFinally
  call sehtestfunct
  add esp, 4
  ret
  
  
section .data
aInFinally: db "In finally",0
aAccessViolatio: db "Access violation",0xD,0xA,0

section .rdata               ; this is crutial to msvcrt!_ValidateEH3RN
_func1_scopetable:           ; can't be in .data
  ; try block 0
  dd -1                      ; EnclosingLevel
  dd 0                       ; FilterFunc
  dd _func1_handler0         ; HandlerFunc
  ; try block 1
  dd 0                       ; EnclosingLevel
  dd _func1_filter1          ; FilterFunc
  dd _func1_handler1         ; HandlerFunc
;The try block 0 has no filter, therefore its handler is a __finally{} block. EnclosingLevel of try block 1 is 0, so it's placed inside try block 0. Considering this, we can try to reconstruct the function structure: 


;    void func1 (char* str)
;    {
;      char buf[12];
;      __try // try block 0
;      {
;         __try // try block 1
;         {
;           *(int*)123=456;
;         }
;         __except(GetExceptCode() =: EXCEPTION_ACCESS_VIOLATION)
;         {
;            printf("Access violation");
;         }
;         strcpy(buf,str);
;      }
;      __finally
;      {
;         puts("in finally");
;      }
;    }

