
#include <Windows.h>
#include <stdio.h>

#include "ShellcodeUtils.h"
#include "payload.h" // referenced in ShellcodeUtils static lib

int main()
{
    ShellcodeUtils::SaveShellcodeForChecking("messagebox");

    ShellcodeUtils user32("user32.dll");
    if (user32) {
        user32.PrintFunctionNameHash("LoadLibraryA");
        user32.PrintFunctionNameHash("MessageBoxA");
        user32.PrintFunctionNameHash("ExitProcess");

        auto msgboxaddr = GetProcAddress(user32, "MessageBoxA");
        auto msgboxaddrbyhash = user32.GetProcAddressByHash(ShellcodeUtils::ComputeNameHash("MessageBoxA"));
        if (msgboxaddr != msgboxaddrbyhash)
            printf("WTF..");
    }

    ShellcodeUtils::ExecuteShellcode();

    return 0;
}
