#pragma once

#include <Windows.h>
#include <Subauth.h>

// Perspective of InInitilizationOrder modules list
struct LDR_MODULE
{
	LIST_ENTRY InInitOrder;        // 0x00
	DWORD DllBase;                 // 0x08
	DWORD EntryPoint;              // 0x0C
	DWORD Reserved;                // 0x10
	UNICODE_STRING FullDllName;    // 0x14
	UNICODE_STRING BaseDllName;    // 0x1C
};