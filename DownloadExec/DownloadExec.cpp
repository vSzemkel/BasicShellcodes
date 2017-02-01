#include <Windows.h>
#include <stdio.h>

#include "payload.h"

void save_shellcode_for_checking()
{
	OFSTRUCT of{ sizeof(OFSTRUCT) };
	auto fh = (HANDLE)OpenFile("..\\IDA\\downloadexec.bin", &of, OF_WRITE | OF_CREATE);
	WriteFile(fh, g_Shellcode, sizeof(g_Shellcode), NULL, NULL);
	CloseHandle(fh);
}

int main()
{
	save_shellcode_for_checking();

    return 0;
}

