#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#undef WIN32_LEAN_AND_MEAN
#include <Shlwapi.h>
//#include <iostream>
#include <string>
//#include <algorithm>

namespace Utils
{
    inline std::string GetWorkingDirectory()
    {
        HMODULE hModule = GetModuleHandle(nullptr);
        if (hModule)
        {
            char path[MAX_PATH];
            GetModuleFileNameA(hModule, path, sizeof(path));
            //PathRemoveFileSpecA(path); ???
            strcat_s(path, "\\");
            return std::string(path);
        }
        return "";
    }
}
