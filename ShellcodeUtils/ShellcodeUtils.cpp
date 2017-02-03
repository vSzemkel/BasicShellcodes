
#include "pch.h"
#include "ShellcodeUtils.h"
#include "pe_export_dir.h"

LPCSTR ShellcodeUtils::binarySaveFmt = "..\\IDA\\%s.bin";

inline void ShellcodeUtils::ROR_0x0D(DWORD& x)
{
	x = x >> 0x0D | x << (0x20 - 0x0D);
}

DWORD ShellcodeUtils::ComputeNameHash(LPCSTR name)
{
	size_t pos = 0;
	DWORD hash = 0;
	while (name[pos] != '\0') {
		ROR_0x0D(hash);
		hash += name[pos++];
	}

	return hash;
}

void ShellcodeUtils::SaveShellcodeForChecking(LPCSTR codename)
{
	CHAR filepath[MAX_PATH];
	sprintf_s(filepath, MAX_PATH, binarySaveFmt, codename);

	OFSTRUCT of{ sizeof(OFSTRUCT) };
	auto fh = (HANDLE)::OpenFile(filepath, &of, OF_WRITE | OF_CREATE);
	WriteFile(fh, g_Shellcode, g_ShellcodeSize, NULL, NULL);
	CloseHandle(fh);
}

void ShellcodeUtils::ExecuteShellcode()
{
	auto sc = VirtualAllocEx(GetCurrentProcess(), 0, g_ShellcodeSize, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	memcpy(sc, g_Shellcode, g_ShellcodeSize);

	void(*c0de)();
	c0de = static_cast<void(*)()>(sc);
	c0de();

	VirtualFreeEx(GetCurrentProcess(), sc, g_ShellcodeSize, MEM_RELEASE);
}

inline BYTE* ShellcodeUtils::RelativeVA2VA(DWORD rva) const
{
	return m_module + rva;
}

FARPROC ShellcodeUtils::GetProcAddressByHash(DWORD hash) const
{
	auto export_dir = GetPeDirectory32(0 /* ExportTable */);
	auto export_tables_struct = reinterpret_cast<EXPORT_DIRECTORY*>(RelativeVA2VA(export_dir->VirtualAddress));
	auto names = reinterpret_cast<DWORD*>(RelativeVA2VA(export_tables_struct->AddressOfNames));
	int number_of_names = export_tables_struct->NumberOfNames;
	while (--number_of_names >= 0) {
		auto fun_name = reinterpret_cast<LPCSTR>(RelativeVA2VA(names[number_of_names]));
		if (hash == ComputeNameHash(fun_name))
			break;
	}

	if (number_of_names < 0)
		return nullptr;

	auto ordinals = reinterpret_cast<WORD*>(RelativeVA2VA(export_tables_struct->AddressOfNameOrdinals));
	auto function_ordinal = ordinals[number_of_names];

	auto addresses = reinterpret_cast<DWORD*>(RelativeVA2VA(export_tables_struct->AddressOfFunctions));
	return reinterpret_cast<FARPROC>(RelativeVA2VA(addresses[function_ordinal]));
}

LPCSTR ShellcodeUtils::GetProcNameByHash(DWORD hash) const
{
	auto export_dir = GetPeDirectory32(0 /* ExportTable */);
	auto export_tables_struct = reinterpret_cast<EXPORT_DIRECTORY*>(RelativeVA2VA(export_dir->VirtualAddress));
	auto names = reinterpret_cast<DWORD*>(RelativeVA2VA(export_tables_struct->AddressOfNames));
	int number_of_names = export_tables_struct->NumberOfNames;
	while (--number_of_names >= 0) {
		auto fun_name = reinterpret_cast<LPCSTR>(RelativeVA2VA(names[number_of_names]));
		if (hash == ComputeNameHash(fun_name))
			return fun_name;
	}

	return nullptr;
}

void ShellcodeUtils::PrintModuleFunctionNameForHash(LPCSTR module_name, DWORD hash)
{
	ShellcodeUtils dll(module_name);
	if (dll) 
		dll.PrintFunctionNameForHash(hash);
	else
		printf("Invalid module %s\n", module_name);
}

void ShellcodeUtils::PrintFunctionNameHash(LPCSTR fun_name)
{
	printf("%-16s hash is 0x%X\n", fun_name, ComputeNameHash(fun_name));
}

void ShellcodeUtils::PrintFunctionNameForHash(DWORD hash) const
{
	auto fun_name = GetProcNameByHash(hash);
	if (fun_name == nullptr)
		printf("There is no function of a name hash 0x%X\n", hash);
	else
		printf("Hash 0x%X is for function %-16s\n", hash, fun_name);
}

IMAGE_NT_HEADERS32* ShellcodeUtils::GetNtHeader32() const
{
	if (m_module == nullptr) 
		return nullptr;

	auto idh = reinterpret_cast<IMAGE_DOS_HEADER*>(m_module);
	if (idh->e_magic != IMAGE_DOS_SIGNATURE) 
		return nullptr;
	
	const LONG kMaxOffset = 1024;
	LONG pe_offset = idh->e_lfanew;
	if (pe_offset > kMaxOffset)
		return nullptr;

	auto inh = reinterpret_cast<IMAGE_NT_HEADERS32*>(m_module + pe_offset);
	return inh;
}

IMAGE_DATA_DIRECTORY* ShellcodeUtils::GetPeDirectory32(DWORD dir_id) const
{
	if (dir_id >= IMAGE_NUMBEROF_DIRECTORY_ENTRIES)
		return nullptr;

	// fetch relocation table from current image:
	auto nt_headers = GetNtHeader32();
	if (nt_headers == nullptr)
		return nullptr;

	IMAGE_DATA_DIRECTORY* peDir = &(nt_headers->OptionalHeader.DataDirectory[dir_id]);
	if (peDir->VirtualAddress == NULL)
		return nullptr;

	return peDir;
}