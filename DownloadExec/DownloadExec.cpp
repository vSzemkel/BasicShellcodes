
#include <Windows.h>
#include <stdio.h>

#include "ShellcodeUtils.h"
#include "payload.h" // referenced in ShellcodeUtils static lib

int main()
{
	ShellcodeUtils::SaveShellcodeForChecking("messagebox");

	ShellcodeUtils::PrintModuleFunctionNameForHash("urlmon.dll", 0x702F1A36);
	ShellcodeUtils::PrintModuleFunctionNameForHash("kernel32.dll", 0x0E8AFE98);
	ShellcodeUtils::PrintModuleFunctionNameForHash("kernelbase.dll", 0x73E2D87E);

	ShellcodeUtils::ExecuteShellcode();

	return 0;
}

