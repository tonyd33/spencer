// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include <iostream>

void Spencer(HMODULE hModule)
{
    AllocConsole();
    FILE* f;
    freopen_s(&f, "CONOUT$", "w", stdout);
    std::cout << "Attached\n";

    while (1)
    {
        if (GetAsyncKeyState(VK_DELETE) & 1) break;
        Sleep(50);
    }

    fclose(f);
    FreeConsole();
    FreeLibraryAndExitThread(hModule, 0);
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    {
        HANDLE thr = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)Spencer, hModule, 0, 0);
        if (thr) CloseHandle(thr);
    }
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

