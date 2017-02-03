.\..\Nasm\nasm -f win32 -g -l ShellcodeHash.lst  ShellcodeHash.asm
link ShellcodeHash.obj libcmt.lib /debug /pdb:ShellcodeHash.pdb
