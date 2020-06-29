// AssaultCubeTrainer.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <iostream>
#include <vector>
#include <Windows.h>
#include "proc.h"
#include "mem.h"

int main()
{

	HANDLE hProcess = 0;

	uintptr_t moduleBase = 0, localPlayerPtr = 0, healthAddr = 0;
	bool bHealth = false, bAmmo = false, bRecoil = false;

	const int newValue = 1337;

	// get the game's process id
	DWORD procId = GetProcId(L"ac_client.exe");

	// If the game's process ID is found
	// Process all access to access and modify game data
	// Calc modulebaseaddress of the game
	// add relative offset to local player pointer
	// use findmaaddy to process multi level pointer and get actual dynamic address of health address
	if (procId) {
		hProcess = OpenProcess(PROCESS_ALL_ACCESS, NULL, procId);

		moduleBase = GetModuleBaseAddress(procId, L"ac_client.exe");

		localPlayerPtr = moduleBase + 0x10f4f4;

		healthAddr = FindDMAAddy(hProcess, localPlayerPtr, { 0xf8 });
	}
	else {
		getchar();
		return 0;
	}

	DWORD dwExit = 0;

	// Declare initial hack menu values
	std::string enabledHealth = "[1] Health Hack | ", enabledAmmo = "[2] Infinite Ammo | ", enabledRecoilHack = "[3] No Recoil";

	// check if the game is still running
	// if so, continue to check for user input
	while (GetExitCodeProcess(hProcess, &dwExit) && dwExit == STILL_ACTIVE) {

		// Print to console the current hacks and if they are enabled
		std::cout << enabledHealth << enabledAmmo << enabledRecoilHack;
		system("cls");

		// Ternary operators to set the menu text to show if the hacks are enabled or not
		(bHealth) ?
			enabledHealth = "[1] Health Hack: ENABLED | " :
			enabledHealth = "[1] Health Hack | ";

		(bAmmo) ?
			enabledAmmo = "[2] Infinite Ammo: ENABLED | " :
			enabledAmmo = "[2] Infinite Ammo | ";

		(bRecoil) ?
			enabledRecoilHack = "[3] No Recoil: ENABLED" :
			enabledRecoilHack = "[3] No Recoil";
		
		// Toggle Health
		if (GetAsyncKeyState(VK_NUMPAD1) & 1) {
			bHealth = !bHealth;
			std::cout << "Health hack: ON";
		}

		// Toggle Ammo
		if (GetAsyncKeyState(VK_NUMPAD2) & 1) {
			bAmmo = !bAmmo;

			if (bAmmo) {
				// ff 06 = inc [esi]
				mem::PatchEx((BYTE*)(moduleBase + 0x637e9), (BYTE*)"\xFF\x06", 2, hProcess);
			}
			else {
				//ff 0E = dec [esi]
				mem::PatchEx((BYTE*)(moduleBase + 0x637e9), (BYTE*)"\xFF\x0E", 2, hProcess);
			}
		}

		// Toggle Recoil
		if (GetAsyncKeyState(VK_NUMPAD3) & 1) {
			bRecoil = !bRecoil;

			if (bRecoil) {
				mem::NopEx((BYTE*)(moduleBase + 0x63786), 10, hProcess);
			}
			else {
				mem::PatchEx((BYTE*)(moduleBase + 0x63786), (BYTE*)"\x50\x8d\x4c\x24\x1c\x51\x8b\xce\xff\xd2", 10, hProcess);
			}
		}

		// Exit Trainer
		if (GetAsyncKeyState(VK_INSERT) & 1) {
			return 0;
		}

		// continuous write of new desired values

		if (bHealth) {
		mem::PatchEx((BYTE*)healthAddr, (BYTE*)&newValue, sizeof(newValue), hProcess);
		}

		// Wait 15 ms between writes
		Sleep(15);
	}

	std::cout << "Process not found! Press enter to exit!";
	getchar();
	return 0;
}