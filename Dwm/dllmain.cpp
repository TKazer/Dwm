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
    static bool Init = false;
    if (!Init)
    {
        if (!MainCom.InitImGui(pDxgiSwapChain))
        {
            OutputDebugStringA("[DWM MainCom] InitImGui() Filed.\n");
            goto END;
        }
        Init = true;
    }

    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("Menu");
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    ImGui::End();

    g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, NULL);
    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
END:
    return oPresentDWM(thisptr, pDxgiSwapChain, a3, a4, a5, a6, a7, a8, a9, a10);
}


UINT WINAPI MainThread(PVOID)
{
    while (!GetModuleHandleA("dwmcore.dll"))
        Sleep(100);
    auto ResolveRelative = [](DWORD64 Address,DWORD StartOffset,DWORD Size) {
        return Address + *reinterpret_cast<int*>(Address + StartOffset) + Size;
    };

    DWORD64 d2d1Module = reinterpret_cast<DWORD64>(GetModuleHandleA("d2d1.dll"));
    DWORD64 ShellCodeMPO = util::FeatureFunc::Search(d2d1Module + 0x1000, "00 00 00 00 00 00 00 00 00 00 00 00");
    DWORD64 CallMPO = 0x13 + util::FeatureFunc::Search(d2d1Module, "48 ?? ?? 48 ?? ?? ?? ?? 8B ?? ?? ?? ?? ?? ?? 89 ?? ?? ?? E8");
    DWORD64 CallDwm = 0x25 + util::FeatureFunc::Search(d2d1Module, "48 8B ?? ?? ?? ?? ?? ?? 48 ?? ?? ?? ?? 8B ?? ?? ?? ?? ?? ?? 89 ?? ?? ?? 48 8B ?? ?? ?? ?? ?? ?? 48 ?? ?? ?? ?? E8");

    if (ShellCodeMPO == 0|| CallMPO == 0 || CallDwm == 0)
        return 0;

    oPresentMPO = reinterpret_cast<PresentMPO_>(ResolveRelative(CallMPO, 1, 5));
    oPresentDWM = reinterpret_cast<PresentDWM_>(ResolveRelative(CallDwm, 1, 5));

    //char Data[256]{};
    //sprintf_s(Data, "EmptyAddress:%llX, CallMPOAddress:%llX, CallDwmAddress:%llX, oPresentMPO:%llX\n, oPresentDWM:%llX\n", ShellCodeMPO, CallMPO, CallDwm, (DWORD64)oPresentMPO,(DWORD64)oPresentDWM);
    //OutputDebugStringA(Data);

    ULONG OldProtect = 0;
    VirtualProtect(reinterpret_cast<PVOID>(ShellCodeMPO), 12, PAGE_EXECUTE_READWRITE, &OldProtect);
    *reinterpret_cast<SHORT*>(ShellCodeMPO) = 0xB848;
    *reinterpret_cast<PVOID*>(ShellCodeMPO + 2) = MyPresentDWM;
    *reinterpret_cast<SHORT*>(ShellCodeMPO + 10) = 0xE0FF;
    VirtualProtect(reinterpret_cast<PVOID>(ShellCodeMPO), 12, OldProtect, &OldProtect);

    VirtualProtect(reinterpret_cast<PVOID>(CallDwm), 10, PAGE_EXECUTE_READWRITE, &OldProtect);
    *reinterpret_cast<DWORD*>(CallDwm + 1) = (ShellCodeMPO - CallDwm - 5);
    VirtualProtect(reinterpret_cast<PVOID>(CallDwm), 10, OldProtect, &OldProtect);
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

