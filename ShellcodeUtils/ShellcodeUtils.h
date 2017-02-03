#pragma once

extern const size_t g_ShellcodeSize;
extern const unsigned char g_Shellcode[];

class ShellcodeUtils
{
public:
	ShellcodeUtils(HMODULE module) : 
		m_module(reinterpret_cast<BYTE*>(module)) {};
	ShellcodeUtils(LPCSTR module_name) :
		m_module(reinterpret_cast<BYTE*>(LoadLibraryA(module_name))) {};

	static void ExecuteShellcode();
	static void SaveShellcodeForChecking(LPCSTR codename);
	static void PrintFunctionNameHash(LPCSTR fun_name);
	static void PrintModuleFunctionNameForHash(LPCSTR module_name, DWORD hash);
	static DWORD ComputeNameHash(LPCSTR name);

	operator bool() const { return m_module != nullptr; };
	operator HMODULE() const { return reinterpret_cast<HMODULE>(m_module); }
	void PrintFunctionNameForHash(DWORD hash) const;
	FARPROC GetProcAddressByHash(DWORD hash) const;
	LPCSTR GetProcNameByHash(DWORD hash) const;
private:
	static LPCSTR binarySaveFmt;
	static void ROR_0x0D(DWORD& x);

	BYTE* m_module;
	BYTE* RelativeVA2VA(DWORD rva) const;
	// from https://github.com/hasherezade/
	IMAGE_NT_HEADERS32* GetNtHeader32() const;
	IMAGE_DATA_DIRECTORY* GetPeDirectory32(DWORD dir_id) const;
	// end from
};