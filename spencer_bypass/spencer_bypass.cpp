#include <iostream>
#include <fstream>
#include <limits>
#include <modlib/modlib.h>
#include "util.h"

// is messing up numeric_limits<>::max
#undef max

#define BLACKCIPHER_HOOK_OFFSET 0x26505e3

bool ReadImageOptionalHeader(LPCSTR loc, PIMAGE_OPTIONAL_HEADER64 header)
{

    std::ifstream file(loc, std::ios::binary);

    if (!file.is_open())
    {
        std::cerr << "Error opening file: " << loc << std::endl;
        return false;
    }

    // Read DOS header (IMAGE_DOS_HEADER)
    IMAGE_DOS_HEADER dosHeader;
    file.read(reinterpret_cast<char*>(&dosHeader), sizeof(dosHeader));

    if (dosHeader.e_magic != IMAGE_DOS_SIGNATURE)
    {
        std::cerr << "Invalid MZ signature" << std::endl;
        return false;
    }

    std::cout << "MZ Header found." << std::endl;
    std::cout << "Offset to PE header: " << std::hex << dosHeader.e_lfanew << std::endl;

    // Move to the PE header location
    file.seekg(dosHeader.e_lfanew, std::ios::beg);

    // Read PE header signature
    DWORD peSignature;
    file.read(reinterpret_cast<char*>(&peSignature), sizeof(peSignature));

    if (peSignature != IMAGE_NT_SIGNATURE)
    {
        std::cerr << "Invalid PE signature" << std::endl;
        return false;
    }

    std::cout << "PE Header found." << std::endl;

    // Read IMAGE_FILE_HEADER
    IMAGE_FILE_HEADER fileHeader;
    file.read(reinterpret_cast<char*>(&fileHeader), sizeof(fileHeader));

    std::cout << "Number of sections: " << fileHeader.NumberOfSections << std::endl;
    std::cout << "Timestamp: " << fileHeader.TimeDateStamp << std::endl;

    // Read IMAGE_OPTIONAL_HEADER
    IMAGE_OPTIONAL_HEADER optionalHeader;
    file.read(reinterpret_cast<char*>(&optionalHeader), sizeof(optionalHeader));

    if (optionalHeader.Magic != IMAGE_NT_OPTIONAL_HDR64_MAGIC)
    {
        std::cerr << "Invalid optional header magic." << std::endl;
        return false;
    }

    std::cout << "Entry point: " << std::hex << optionalHeader.AddressOfEntryPoint << std::endl;
    std::cout << "Image base: " << std::hex << optionalHeader.ImageBase << std::endl;
    std::cout << "Size of image: " << optionalHeader.SizeOfImage << std::endl;
    std::cout << "Base of code: " << std::hex << optionalHeader.BaseOfCode << std::endl;
    std::cout << "Size of code: " << optionalHeader.SizeOfCode << std::endl;

    file.close();

    return true;
}

struct MapleInfo
{
    HANDLE hProcMaple;
    DWORD procIdMaple;
    uintptr_t baseMaple;

    HANDLE hProcBlackCipher;
    DWORD procIdBlackCipher;
    uintptr_t baseBlackCipher;
};

bool FindMaple(MapleInfo* mi)
{
    // blackcipher
    HANDLE hProcBlackCipher;;
    DWORD procIdBlackCipher = Util::GetProcId(L"BlackCipher64.aes");

    if (procIdBlackCipher == NULL)
        return false;

    hProcBlackCipher = OpenProcess(PROCESS_ALL_ACCESS, 0, procIdBlackCipher);
    if (hProcBlackCipher == NULL)
        return false;

    uintptr_t baseBlackCipher = Util::GetModuleBaseAddr(procIdBlackCipher, L"BlackCipher64.aes");
    if (baseBlackCipher == 0)
        return false;


    // maple
    HANDLE hProcMaple;
    DWORD procIdMaple = Util::GetProcId(L"maplestory.exe");

    if (procIdMaple == NULL)
        return false;

    hProcMaple = OpenProcess(PROCESS_ALL_ACCESS, 0, procIdMaple);
    if (hProcMaple == NULL)
        return false;

    uintptr_t baseMaple = Util::GetModuleBaseAddr(procIdMaple, L"maplestory.exe");
    if (baseMaple == 0)
        return false;


    mi->hProcMaple = hProcMaple;
    mi->procIdMaple = procIdMaple;
    mi->baseMaple = baseMaple;

    mi->hProcBlackCipher = hProcBlackCipher;
    mi->procIdBlackCipher = procIdBlackCipher;
    mi->baseBlackCipher = baseBlackCipher;

    return true;

}

void CRCBypassMaplestory(Util::HookManager& hm, MapleInfo& mi)
{

}

void WhiteCipher(Util::HookManager& hm, MapleInfo& mi)
{
    std::map<std::string, uintptr_t> exportsMap;
    uintptr_t kernel32Addr = Util::GetModuleBaseAddr(mi.procIdMaple, L"KERNEL32.dll");

    if(!Util::GetModuleExports(kernel32Addr, exportsMap))
        throw std::range_error("Couldn't get KERNEL32.dll exports");

    if (!exportsMap.contains("Sleep"))
        throw std::range_error("Couldn't find Sleep from KERNEL32.dll");

    uintptr_t sleepAddr = exportsMap["Sleep"];

    Util::BytesAssembler strm;
    constexpr uint64_t bigNum = std::numeric_limits<uint64_t>::max();

    strm
        << "\x48\xb9"s << bigNum    // mov rcx, bigNum
        << "\x51"s                  // push rcx
        << "\x48\xbe"s << sleepAddr // mov rsi, sleepAddr
        << "\xff\xe6"s              // jmp rsi
        ;
    
    std::vector<unsigned char> assembly(strm.data(), strm.data() + strm.size());
    hm.AssemblyHookCreate(
        mi.hProcBlackCipher,
        mi.baseBlackCipher + BLACKCIPHER_HOOK_OFFSET,
        assembly,
        0x18,
        false,
        false
    );
}


int main()
{
    LPCSTR maplestoryLoc = "C:\\Nexon\\Library\\maplestory\\appdata\\MapleStory.exe";
    IMAGE_OPTIONAL_HEADER64 header;
    if (!ReadImageOptionalHeader(maplestoryLoc, &header))
    {
        std::cerr << "Fatal error: failed to read image optional header. Exiting now." << std::endl;
        return 1;
    }

    std::cout << "Waiting for maplestory+BlackCipher to come up..." << std::endl;
    MapleInfo mi;
    while (!FindMaple(&mi)) Sleep(10);
    std::cout << "Found maplestory+BlackCipher" << std::endl;
    std::cout << "Injecting bypasses" << std::endl;

    Util::HookManager hm;
    WhiteCipher(hm, mi);

    hm.HookPrepareAll();
    hm.HookEnableAll();

    while (true)
    {
        if (GetAsyncKeyState(VK_F9) & 1) break;

        Sleep(50);
    }

    hm.HookDeleteAll();

    return 0;
}

