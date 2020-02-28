#include <iostream>
#include <windows.h>
#include <vector>
#include "proc.h"

// DO NOT USING NAMESPACE STD!!!!!!!!!!!!!!!!!!!!!!!!!
int main() {

	// ========================== get process ID of target process==========================
	DWORD procId = GetProcId(L"ac_client.exe"); // LETTER 'L' Transforms our unicode "ac_client.exe" to char array

	// ========================== get module base address==========================
	uintptr_t moduleBase = GetModuleBaseAddress(procId, L"ac_client.exe");

	//========================== get handle for process==========================
	HANDLE hProcess = nullptr; // always good to initialize things to 0 for error checking
	hProcess = OpenProcess(PROCESS_ALL_ACCESS, false, procId);

	//========================== resolve base address of the pointer chain===================
/*
	THIS IS BASE(static) address from Cheat engine also called "ac_client" + pointer address;
	in cheat engine when u click on pointers address its gonna look like this:
	ac_client.exe+10F4F4
	for ammo specific its gonna look like this:
	https://i.imgur.com/E4MOI0X.png where 150 is offset from  "ac_client" + (resolved)pointer address + 150;
*/
	uintptr_t dynamicPtrBaseAddr = moduleBase + 0x10F4F4; //  THIS IS BASE(static) address from Cheat engine also called "ac_client" + pointer address;
	std::cout << "DYNAMIC PTR BASE ADDRESS = 0x" << std::hex << dynamicPtrBaseAddr << std::endl;

	// ==================== resolve ammo pointer chain==================
	std::vector<unsigned int> ammoOffsets = { 0x150 }; //not sure https://youtu.be/wiX5LmdD5yk?t=2137
	uintptr_t ammoAddr = FindDMMAAddy(hProcess, dynamicPtrBaseAddr, ammoOffsets);
	std::cout << "DYNAMIC AMMO ADDRESS = 0x" << std::hex << ammoAddr << std::endl;

	//==================== read ammo value ====================
	int ammoValue = 0;
	ReadProcessMemory(hProcess, (BYTE*)ammoAddr, &ammoValue, sizeof(ammoValue), nullptr);
	std::printf("[RPM1]CURRENT AMMO: %i\n", ammoValue);

	//==================== write ====================
	int newAmmo = 1337;
	WriteProcessMemory(hProcess, (BYTE*)ammoAddr, &newAmmo, sizeof(newAmmo), nullptr); // gonna write 4 bytes into address

	//==================== read ammo value(again) ====================
	int ammoValue2 = 0;
	ReadProcessMemory(hProcess, (BYTE*)ammoAddr, &ammoValue2, sizeof(ammoValue2), nullptr);
	std::printf("[RPM2]CURRENT AMMO: %i\n", ammoValue2);

	getchar(); // require to put enter before ends
	return 0;
}
