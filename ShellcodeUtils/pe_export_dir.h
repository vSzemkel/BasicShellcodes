#pragma once

#include <Windows.h>

struct EXPORT_DIRECTORY
{
    DWORD Characteristics;         // 0x00
    DWORD TimeDateStamp;           // 0x04
    WORD MajorVersion;             // 0x08
    WORD MinorVersion;             // 0x0A
    DWORD NameRVA;                 // 0x0C
    DWORD Base;                    // 0x10
    DWORD NumberOfFunctions;       // 0x14
    DWORD NumberOfNames;           // 0x18
    DWORD AddressOfFunctions;      // 0x1C
    DWORD AddressOfNames;          // 0x20
    DWORD AddressOfNameOrdinals;   // 0x24
};