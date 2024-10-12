#pragma once

#include <windows.h>
#include <vector>
#include <string>

struct Drive
{
	std::string rootPath;
	// total drive capacity in megabytes
	DWORD totalSize;
	// total available space in megabytes
	DWORD availableSpace;
};

LSTATUS GetMachineGuid(LPSTR guid);
std::vector<std::string> GetAvailableDrives();
std::string GetProcessorName();
bool CheckKillswitch();