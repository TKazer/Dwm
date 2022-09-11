#include <windows.h>
#include <process.h>
#include "OwnImGui.h"
#include "util.h"
#include "Com.h"

using PresentMPO_ = __int64(__fastcall*)(void*, IDXGISwapChain*, unsigned int, unsigned int, int, __int64, __int64, int);
using PresentDWM_ = __int64(__fastcall*)(void*, IDXGISwapChain*, unsigned int, unsigned int, const struct tagRECT*, unsigned int, const struct DXGI_SCROLL_RECT*, unsigned int, struct IDXGIResource*, unsigned int);
PresentMPO_ oPresentMPO = NULL;
PresentDWM_ oPresentDWM = NULL;

CMainCom MainCom;

__int64 __fastcall MyPresentMPO(void* thisptr, IDXGISwapChain* pDxgiSwapChain, unsigned int a3, unsigned int a4, int a5, __int64 a6, __int64 a7, int a8)
{
    static bool Init = false;
    if (!Init)
    {
        if (!MainCom.InitImGui(pDxgiSwapChain))
        {
            OutputDebugStringA("[DWM MainCom] InitImGui() Filed.\n");
            goto END;
        }
        if (!MainCom.InitMessageMap(L"Windows Handler"))
        {
            OutputDebugStringA("[DWM MainCom] InitMessageMap() Filed.\n");
            goto END;
        }
        Init = true;
    }
    MainCom.Com();
    
    END:
    return oPresentMPO(thisptr, pDxgiSwapChain, a3, a4, a5, a6, a7, a8);
}

__int64 __fastcall MyPresentDWM(void* thisptr, IDXGISwapChain* pDxgiSwapChain, unsigned int a3, unsigned int a4, const struct tagRECT* a5, unsigned int a6, const struct DXGI_SCROLL_RECT* a7, unsigned int a8, struct IDXGIResource* a9, unsigned int a10)
{
    // ...
    return oPresentDWM(thisptr, pDxgiSwapChain, a3, a4, a5, a6, a7, a8, a9, a10);
}


UINT WINAPI MainThread(PVOID)
{
    while (!GetModuleHandleA("dwmcore.dll"))
        Sleep(100);
    auto ResolveRelative = [](DWORD64 Address) {
        return Address + *reinterpret_cast<int*>(Address + 0x3) + 0x7;
    };

    DWORD64 d2d1Module = reinterpret_cast<DWORD64>(GetModuleHandleA("d2d1.dll"));
    DWORD64 ShellCodeMPO = util::FeatureFunc::Search(d2d1Module + 0x1000, "00 00 00 00 00 00 00 00 00 00 00 00");
    DWORD64 DrawingContext = util::FeatureFunc::Search(d2d1Module, "48 8D 05 ?? ?? ?? ?? 33 ED 48 8D 71 08");
    DWORD64 CallMPO = 24 + util::FeatureFunc::Search(d2d1Module, "48 89 4C 24 30 48 8B CB 48 89 44 24 28 8B 84 24 80 00 00 00 89 44 24 20");
    DWORD64 CallDwm = 24 + util::FeatureFunc::Search(d2d1Module, "8B 84 24 98 00 00 00 89 44 24 28 48 8B 84 24 90 00 00 00 48 89 44 24 20");

    if (ShellCodeMPO == 0 || DrawingContext == 0 || CallMPO == 0 || CallDwm == 0)
        return 0;

    DWORD64 VfTable = ResolveRelative(DrawingContext);
    oPresentMPO = reinterpret_cast<PresentMPO_>(*(PVOID*)(VfTable + 7 * 0x8));

    //char Data[256]{};
    //sprintf_s(Data, "EmptyAddress:%llX, CallMPOAddress:%llX, CallDwmAddress:%llX, oPresentMPO:%llX\n", ShellCodeMPO, CallMPO, CallDwm, (DWORD64)oPresentMPO);
    //OutputDebugStringA(Data);

    ULONG OldProtect = 0;
    VirtualProtect(reinterpret_cast<PVOID>(ShellCodeMPO), 12, PAGE_EXECUTE_READWRITE, &OldProtect);
    *reinterpret_cast<SHORT*>(ShellCodeMPO) = 0xB848;
    *reinterpret_cast<PVOID*>(ShellCodeMPO + 2) = MyPresentMPO;
    *reinterpret_cast<SHORT*>(ShellCodeMPO + 10) = 0xE0FF;
    VirtualProtect(reinterpret_cast<PVOID>(ShellCodeMPO), 12, OldProtect, &OldProtect);

    VirtualProtect(reinterpret_cast<PVOID>(CallMPO), 10, PAGE_EXECUTE_READWRITE, &OldProtect);
    *reinterpret_cast<DWORD*>(CallMPO + 1) = (ShellCodeMPO - CallMPO - 5);
    VirtualProtect(reinterpret_cast<PVOID>(CallMPO), 10, OldProtect, &OldProtect);
    return 0;
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        _beginthreadex(nullptr, NULL, MainThread, nullptr, NULL, nullptr);
        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

