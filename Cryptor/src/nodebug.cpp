#include <kitty/nodebug.h>
#include <kitty/system.h>
#include <adv/MetaString.h>
#include <windows.h>

static bool CheckDebuggers()
{
	if (IsDebuggerPresent())
		return true;

	HANDLE hProcess = GetCurrentProcess();
	BOOL remoteDebuggerPresent;

	if (CheckRemoteDebuggerPresent(hProcess, &remoteDebuggerPresent) == 0)
		return true;

	if (remoteDebuggerPresent)
		return true;

	return false;
}

static bool CheckVM()
{
	HKEY hKey{};
	LSTATUS status{};

	status = RegOpenKeyEx(
		HKEY_CLASSES_ROOT,
		OBFUSCATED("Applications\\VMwareHostOpen.exe"),
		0,
		KEY_QUERY_VALUE,
		&hKey
	);

	if (status == ERROR_SUCCESS)
		return true;

	return false;
}

bool ShouldQuit()
{
	if (CheckDebuggers())
		return true;

	if (CheckVM())
		return true;

	return false;
}