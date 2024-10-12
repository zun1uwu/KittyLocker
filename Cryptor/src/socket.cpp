#include <kitty/socket.h>
#include <kitty/log.h>

bool SendPost(std::string remotePath, nlohmann::json json)
{
	std::string remoteAddress{ OBFUSCATED("127.0.0.1:7787") };
	unsigned short remotePort{ 7787 };

	int sock{};
	struct sockaddr_in sockAddr {};
	WSADATA wsaData{};

	if (WSAStartup(MAKEWORD(2, 0), &wsaData) != 0)
	{
		KittyLog(OBFUSCATED("Unable to start WSA"), false);
		return false;
	}

	if ((sock = (int)socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
	{
		KittyLog(OBFUSCATED("Failed to open socket"), false);
		return false;
	}

	memset(&sockAddr, 0, sizeof(sockAddr));
	sockAddr.sin_family = AF_INET;
	sockAddr.sin_addr.s_addr = inet_addr(OBFUSCATED("127.0.0.1"));
	sockAddr.sin_port = htons((unsigned short)remotePort);

	if (connect(sock, (struct sockaddr*)&sockAddr, sizeof(sockAddr)) < 0)
	{
		KittyLog(OBFUSCATED("Failed to connect"), false);
		return false;
	}

	std::string request{};
	std::string data{ json.dump() };

	std::stringstream requestStream{};
	requestStream << OBFUSCATED("POST ") << remotePath << OBFUSCATED(" HTTP/1.1\n");
	requestStream << OBFUSCATED("Host: ") << remoteAddress << ":" << std::to_string(remotePort) << "\n";
	requestStream << OBFUSCATED("User-Agent: Mozilla/5.0\n");
	requestStream << OBFUSCATED("Content-Type: application/json\n");
	requestStream << OBFUSCATED("Content-Length: ") << data.length() << "\n";
	requestStream << OBFUSCATED("Accept-Language: en-us,en\n");
	requestStream << "\n";
	requestStream << data;
	request = requestStream.str();

	std::string response = Send(request, sock);

	closesocket(sock);
	WSACleanup();

	return response.find(OBFUSCATED("200 OK")) != std::string::npos;
}

static std::string Send(std::string request, int sock)
{
	if (send(sock, request.c_str(), (int)request.length(), 0) != request.length())
	{
		KittyLog(OBFUSCATED("Unexpected number of bytes sent"), false);
		return "";
	}
	
	return Recv(sock);
}

static std::string Recv(int sock)
{
	char buffer[bufferSize]{};
	std::string data{};
	int bytesLen{ bufferSize };

	while (bytesLen == bufferSize)
	{
		bytesLen = recv(sock, (char*)&buffer, bufferSize, 0);

		if (bytesLen > 0)
			data += std::string(buffer).substr(0, bytesLen);
	}

	return data;
}