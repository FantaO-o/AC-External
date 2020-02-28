//
// Created by marco on 26/02/2020.
//

#include "proc.h"

/*
 * FINDING PROCESS ID OF A GIVEN NAME
 * https://www.youtube.com/watch?v=wiX5LmdD5yk&feature=emb_title
 */
DWORD GetProcId(const wchar_t* procName) {

	DWORD procID = 0;
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0); // get snapshots of all running processes

	if (hSnap != INVALID_HANDLE_VALUE) {
		PROCESSENTRY32 procEntry; // like and iterator, we define one object thats gonna get assigned and check if process name equals ours
		procEntry.dwSize = sizeof(procEntry);

		if (Process32First(hSnap, &procEntry)) {

			do {

				if (!_wcsicmp(procEntry.szExeFile, procName)) { // _wcsicmp = string compare case insensitive
					procID = procEntry.th32ProcessID;
					break;
				}

			} while (Process32Next(hSnap, &procEntry));


		}

	}
	CloseHandle(hSnap);
	return procID;

}


// uintptr_t is like and platform(x86 or x64) independent variable
uintptr_t GetModuleBaseAddress(DWORD procId, const wchar_t* modName) {
	uintptr_t  modBaseAddress = 0;
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, procId);

	if (hSnap != INVALID_HANDLE_VALUE) {
		MODULEENTRY32 modEntry;
		modEntry.dwSize = sizeof(modEntry);

		if (Module32First(hSnap, &modEntry)) {

			do {
				if (!_wcsicmp(modEntry.szModule, modName)) {
					modBaseAddress = (uintptr_t)(modEntry.modBaseAddr);
					break;
				}


			} while (Module32Next(hSnap, &modEntry));

		}

	}
	CloseHandle(hSnap);
	return modBaseAddress;

}


/*
 * FIND DYNAMIC ALLOCATED MEMORY ADDRESS
 * @param ptr is pointer to our process e.g. AssaultCube
*/
uintptr_t FindDMMAAddy(HANDLE hProc, uintptr_t ptr, std::vector<unsigned int> offsets) {
	uintptr_t  addr = ptr;

	for (unsigned int offset : offsets) {
		BOOL rpmBool = ReadProcessMemory(hProc, (BYTE*)addr, &addr, sizeof(addr), nullptr);
		if (rpmBool == false) {
			printf("SOMETHING WENT WRONG IN RPM: %lu\n", GetLastError());
		}
		addr += offset;
	}
	return addr; // returns dynamic address of actual ammo value
}