
#include <Windows.h>
#include <stdio.h>

#include "payload.h"
#include "pe_hdrs_helper.h"
#include "pe_export_dir.h"

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
	auto fh = (HANDLE)OpenFile("..\\IDA\\shellcode.bin", &of, OF_WRITE|OF_CREATE);
	WriteFile(fh, g_Shellcode, sizeof(g_Shellcode), NULL, NULL);
	CloseHandle(fh);
}

void ror_0x0D(DWORD& x)
{
	x = x >> 0x0D | x << (0x20 - 0x0D);
}

DWORD compute_hash(LPCSTR name)
{
	size_t pos = 0;
	DWORD hash = 0;
	while (name[pos] != '\0') {
		ror_0x0D(hash);
		hash += name[pos++];
	}

	return hash;
}

void print_function_name_hash(LPCSTR fun_name)
{
	printf("%-16s hash is 0x%X\n", fun_name, compute_hash(fun_name));
}

byte* rva2va(HMODULE module, DWORD rva)
{
	return (byte*)module + rva;
}

FARPROC get_proc_address_by_hash(HMODULE module, DWORD hash)
{
	auto export_dir = get_pe_directory32(module, 0 /* ExportTable */);
	auto fun_count = export_dir->Size;
	auto export_tables_struct = reinterpret_cast<EXPORT_DIRECTORY*>(rva2va(module, export_dir->VirtualAddress));
	auto names = reinterpret_cast<DWORD*>(rva2va(module, export_tables_struct->AddressOfNames));
	int number_of_names = export_tables_struct->NumberOfNames;
	while (--number_of_names >= 0) {
		auto fun_name = reinterpret_cast<LPCSTR>(rva2va(module, names[number_of_names]));
		if (hash == compute_hash(fun_name))
			break;
	}

	if (number_of_names < 0)
		return NULL;

	auto ordinals = reinterpret_cast<WORD*>(rva2va(module, export_tables_struct->AddressOfNameOrdinals));
	auto function_ordinal = ordinals[number_of_names];

	auto addresses = reinterpret_cast<DWORD*>(rva2va(module, export_tables_struct->AddressOfFunctions));
	return reinterpret_cast<FARPROC>(rva2va(module, addresses[function_ordinal]));
}

int main()
{
	save_shellcode_for_checking();

	print_function_name_hash("LoadLibraryA");
	print_function_name_hash("MessageBoxA");
	print_function_name_hash("ExitProcess");

	auto user32lib = LoadLibraryA("user32.dll");
	auto msgboxaddr = GetProcAddress(user32lib, "MessageBoxA");
	auto msgboxaddrbyhash = get_proc_address_by_hash(user32lib, compute_hash("MessageBoxA"));
	if (msgboxaddr != msgboxaddrbyhash)
		printf("WTF..");

	execute_shellcode();

	return 0;
}
