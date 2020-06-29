#pragma once
#include <windows.h>

namespace mem
{
	// Update an instruction in memory
	// Used to patch health value and to increment ammo value
	void PatchEx(BYTE* dst, BYTE* src, unsigned int size, HANDLE hProcess);

	// Cancel an instruction in memory
	// Used to remove recoil
	void NopEx(BYTE* dst, unsigned int size, HANDLE hProcess);
}