.\..\..\bin\nasm -f win32 -g -l SafeSEH.lst  SafeSEH.asm
link SafeSEH.obj /debug /pdb:SafeSEH.pdb
