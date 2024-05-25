#include <windows.h>
#include <psapi.h>
#include <dbghelp.h>
#include <iostream>
#include <vector>
#include <map>

#pragma comment(lib, "psapi.lib")
#pragma comment(lib, "dbghelp.lib")

namespace Util
{
    bool GetModuleExports(uintptr_t baseAddress, std::map<std::string, uintptr_t>& exportsMap)
    {
        // Get the NT headers
        PIMAGE_NT_HEADERS ntHeaders = ImageNtHeader((void*)baseAddress);
        if (!ntHeaders)
        {
            std::cerr << "Failed to get NT headers: " << GetLastError() << std::endl;
            return false;
        }

        // Get the export directory
        DWORD exportSize;
        PIMAGE_EXPORT_DIRECTORY exportDirectory = (PIMAGE_EXPORT_DIRECTORY)ImageDirectoryEntryToData(
            (void*)baseAddress, TRUE, IMAGE_DIRECTORY_ENTRY_EXPORT, &exportSize);

        if (!exportDirectory)
        {
            std::cerr << "No export directory found" << std::endl;
            return false;
        }

        // Get the address of the functions, names and ordinals
        DWORD* functions = (DWORD*)((BYTE*)baseAddress + exportDirectory->AddressOfFunctions);
        DWORD* names = (DWORD*)((BYTE*)baseAddress + exportDirectory->AddressOfNames);
        WORD* ordinals = (WORD*)((BYTE*)baseAddress + exportDirectory->AddressOfNameOrdinals);

        // Iterate over the export names and populate the map
        for (DWORD i = 0; i < exportDirectory->NumberOfNames; ++i)
        {
            char* functionName = (char*)((BYTE*)baseAddress + names[i]);
            DWORD functionRVA = functions[ordinals[i]];
            void* functionAddress = (BYTE*)baseAddress + functionRVA;
            exportsMap[functionName] = (uintptr_t)functionAddress;
        }

        return true;
    }
}

