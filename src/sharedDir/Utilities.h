#pragma once
#define RUNNING_WINDOWS
#include <SFML/Network.hpp>
#include <sstream>
#include <iostream>
#include <string>
#include <algorithm>
namespace Utils {
#ifdef RUNNING_WINDOWS
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <Shlwapi.h>
	inline std::string GetWorkingDirectory() {
		HMODULE hModule = GetModuleHandle(nullptr);
		if (hModule) {
			char path[256];
			GetModuleFileName(hModule, LPSTR(path), sizeof(path));
			PathRemoveFileSpec(LPSTR(path));
			strcat_s(path, ":/");
			return std::string(path);
		}
		return "";
	}
#elif defined RUNNING_LINUX
#include <unistd.h>
	inline std::string GetWorkingDirectory() {
		char cwd[1024];
		if (getcwd(cwd, sizeof(cwd)) != nullptr) {
			return std::string(cwd) + std::string("/");
		}
		return "";
	}
#endif
	inline std::string GetResourceDirectory() {
		return GetWorkingDirectory() + std::string("Resources/");
	}
	inline void ReadQuotedString(std::stringstream& _stream, std::string& _string) {
		_stream >> _string;
		if (_string.at(0) == '"') {
			while (_string.at(_string.length() - 1) != '"' || !_stream.eof()) {
				std::string str;
				_stream >> str;
				_string.append(" " + str);
			}
		}
		_string.erase(std::remove(_string.begin(), _string.end(), '"'), _string.end());
	}
}