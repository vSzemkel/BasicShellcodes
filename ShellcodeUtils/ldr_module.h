#pragma once

#include <Windows.h>
#include <Subauth.h>

// Perspective of InInitilizationOrder modules list
struct LDR_MODULE
{
    LIST_ENTRY InInitOrder;        // 0x00
    VOID* DllBase;                 // 0x08
    VOID* EntryPoint;              // 0x0C
    ULONG SizeOfImage;             // 0x10
    UNICODE_STRING FullDllName;    // 0x14
    UNICODE_STRING BaseDllName;    // 0x1C
    ULONG   Flags;
    SHORT   LoadCount;
    SHORT   TlsIndex;
    HANDLE  SectionHandle;
    ULONG   CheckSum;
    ULONG   TimeDateStamp;
};
