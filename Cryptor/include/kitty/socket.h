#pragma once

#include <adv/MetaString.h>
#include <nlohmann/json.hpp>

#pragma comment (lib, "wsock32.lib")
#include <winsock.h>

constexpr int bufferSize = 1024;

bool SendPost(std::string path, nlohmann::json json);
std::string Send(std::string request, int sock);
std::string Recv(int sock);