#include <Windows.h>
#include <iostream>
#include <string>
#include <stdexcept>

#include "hooks.h"   
#include "cheats.h"  
#include "SDK.hpp"   

HMODULE g_hModule = nullptr;

DWORD WINAPI MainThread(LPVOID lpReserved) {
    AllocConsole();
    FILE* pFile = nullptr;
    freopen_s(&pFile, "CONOUT$", "w", stdout);
    std::cout << "[+] MainThread started." << std::endl;


    if (hooks::init_hooks()) {
        std::cout << "[+] Hooks initialized successfully. Press DELETE to eject." << std::endl;

        while (!(GetAsyncKeyState(VK_DELETE) & 0x8000)) {

            Cheats::UpdateActors();

            Sleep(250);
        }
        std::cout << "[-] DELETE key pressed. Shutting down..." << std::endl;
    }
    else {
        std::cout << "[!] Failed to initialize hooks." << std::endl;
    }

    hooks::shutdown_hooks();
    std::cout << "[-] Hooks shut down." << std::endl;

    if (pFile) fclose(pFile);
    FreeConsole();
    FreeLibraryAndExitThread(g_hModule, 0);
    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    if (ul_reason_for_call == DLL_PROCESS_ATTACH) {
        g_hModule = hModule;
        DisableThreadLibraryCalls(hModule);
        if (HANDLE hThread = CreateThread(nullptr, 0, MainThread, hModule, 0, nullptr)) {
            CloseHandle(hThread);
        }
    }
    return TRUE;
}
