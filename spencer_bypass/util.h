#pragma once
#include <Windows.h>
#include <string>
#include <map>

// TODO: port this into modlib
namespace Util
{
    bool GetModuleExports(uintptr_t modAddr, std::map<std::string, uintptr_t>& exportsMap);
}
