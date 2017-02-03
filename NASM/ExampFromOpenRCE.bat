.\..\Nasm\nasm -f win32 -g -l ExampFromOpenRCE.lst  ExampFromOpenRCE.asm
link /safeseh ExampFromOpenRCE.obj /debug /pdb:ExampFromOpenRCE.pdb
