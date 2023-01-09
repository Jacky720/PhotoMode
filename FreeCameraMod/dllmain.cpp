// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include "FreeCamera.h"

DWORD WINAPI thing(LPVOID) noexcept
{
    while (true)
    {
        static bool once = false;
        if (!once)
        {
            FreeCamera::LoadConfig();
            once = true;
        }
        FreeCamera::Run();
        Sleep(10u);
    }
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    DisableThreadLibraryCalls(hModule);
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        CreateThread(0, NULL, (LPTHREAD_START_ROUTINE)&thing, 0, NULL, 0);
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

