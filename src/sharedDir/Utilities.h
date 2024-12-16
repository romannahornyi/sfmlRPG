#pragma once
#define RUNNING_WINDOWS
#include <SFML/Network.hpp>
#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>

namespace Utils {
    #ifdef RUNNING_WINDOWS
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
    #include <shlwapi.h>
    inline std::string GetWorkingDirectory() {
        HMODULE hModule = GetModuleHandle(nullptr);
        if (hModule) {
            char path[256];
            GetModuleFileName(hModule, (LPWSTR)path, sizeof(path));
            PathRemoveFileSpec((LPWSTR)path);
            strcat_s(path, "\\");
            return std::string(path);
        }
        return "";
    };
    #elif defined RUNNING_LINUX
    #include <unistd.h>
    inline std::string GetWorkingDirectory() {
        char cwd[1024];
        if (getcwd(cwd, sizeof(cwd)) != nullptr) {
            return std::string(cwd) + std::string("/");
        }
        return "";
    };
    #endif
    inline void ReadQuotedString(std::stringstream& _stream, std::string& _str) {
        _stream >> _str;
        if (_str.at(0) == '"') {
            while (_str.at(_str.length() - 1) != '"' || !_stream.eof()) {
                std::string s;
                _stream >> s;
                _str.append(" " + s);
            }
        }
        _str.erase(std::remove(_str.begin(), _str.end(), '"'), _str.end());
    };
};