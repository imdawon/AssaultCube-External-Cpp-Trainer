// AssaultCubeTrainer.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <iostream>
#include <vector>
#include <Windows.h>
#include "proc.h"

int main()
{
    //Get ProcId of the target process
    DWORD procId = GetProcId(L"ac_client.exe");

    //Getmodulebaseaddress
    uintptr_t moduleBase = GetModuleBaseAddress(procId, L"ac_client.exe");

    //Get Handle to Process
    HANDLE hProcess = 0;
    hProcess = OpenProcess(PROCESS_ALL_ACCESS, NULL, procId);

    //Resolve base address of the pointer chain
    uintptr_t dynamicPtrBaseAddr = moduleBase + 0x10f4f4;

    std::cout << "DynamicPtrBaseAddr = " << "0x" << std::hex << dynamicPtrBaseAddr << std::endl;

    //Resolve our ammo pointer chain
    std::vector<unsigned int> ammoOffsets = { 0x374, 0x14, 0x0 };
    uintptr_t ammoAddr = findDMAAddy(hProcess, dynamicPtrBaseAddr, ammoOffsets);

    std::cout << "ammoAddr = " << "0x" << std::hex << ammoAddr << std::endl;

    //Read Ammo value
    int ammoValue = 0;

    ReadProcessMemory(hProcess, (BYTE*)ammoAddr, &ammoValue, sizeof(ammoValue), nullptr);
    std::cout << "Curent ammo = " << std::dec << ammoValue << std::endl;

    //Write to it
    int newAmmo = 1337;
    WriteProcessMemory(hProcess, (BYTE*)ammoAddr, &newAmmo, sizeof(newAmmo), nullptr);

    //Read out again
    ReadProcessMemory(hProcess, (BYTE*)ammoAddr, &ammoValue, sizeof(ammoValue), nullptr);

    std::cout << "New ammo = " << std::dec << ammoValue << std::endl;

    getchar();

    return 0;
}