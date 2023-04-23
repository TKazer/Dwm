#pragma once
#include "Define.h"
#include "Com.h"

CMainCom MainCom;

__int64 __fastcall MyPresentMPO(void* thisptr, IDXGISwapChain* pDxgiSwapChain, unsigned int a3, unsigned int a4, int a5, __int64 a6, __int64 a7, int a8)
{
    static bool Init = false;
    if (!Init)
    {
        if (!MainCom.InitImGui(pDxgiSwapChain))
        {
            OutputDebugStringA(("[DWM MainCom] InitImGui() Failed.\n"));
            goto END;
        }
        if (!MainCom.InitMessageMap(XOR(L"Windows Handler")))
        {
            OutputDebugStringA(("[DWM MainCom] InitMessageMap() Failed.\n"));
            goto END;
        }
        Init = true;
    }
    MainCom.Com();

END:
    return util::Call<__int64>(DwmEntry::oPresentMPO, thisptr, pDxgiSwapChain, a3, a4, a5, a6, a7, a8);
}

__int64 __fastcall MyPresentDWM(void* thisptr, IDXGISwapChain* pDxgiSwapChain, unsigned int a3, unsigned int a4, const struct tagRECT* a5, unsigned int a6, const struct DXGI_SCROLL_RECT* a7, unsigned int a8, struct IDXGIResource* a9, unsigned int a10)
{
    static bool Init = false;
    if (!Init)
    {
        if (!MainCom.InitImGui(pDxgiSwapChain))
        {
            OutputDebugStringA(("[DWM MainCom] InitImGui() Failed.\n"));
            goto END;
        }
        if (!MainCom.InitMessageMap(XOR(L"Windows Handler")))
        {
            OutputDebugStringA(("[DWM MainCom] InitMessageMap() Failed.\n"));
            goto END;
        }
        Init = true;
    }
    MainCom.Com();

END:
    return util::Call<__int64>(DwmEntry::oPresentDWM, thisptr, pDxgiSwapChain, a3, a4, a5, a6, a7, a8, a9, a10);
}

__int64 __fastcall MyPresentMPO_W11(void* thisptr, IDXGISwapChain* pDxgiSwapChain)
{
    static bool Init = false;
    if (!Init)
    {
        if (!MainCom.InitImGui(pDxgiSwapChain))
        {
            OutputDebugStringA(("[DWM MainCom] InitImGui() Failed.\n"));
            goto END;
        }
        if (!MainCom.InitMessageMap((L"Windows Handler")))
        {
            OutputDebugStringA(("[DWM MainCom] InitMessageMap() Failed.\n"));
            goto END;
        }
        Init = true;
    }
    MainCom.Com();

END:
    return util::Call<__int64>(DwmEntry::oPresentMPOW11, thisptr, pDxgiSwapChain);//oPresentMPOW11(thisptr, pDxgiSwapChain);
}

namespace DwmEntry
{
    void Start()
    {
        auto ResolveRelative = [](DWORD64 Address, DWORD StartOffset, DWORD Size) {
            return Address + *reinterpret_cast<int*>(Address + StartOffset) + Size;
        };

        // Build获取
        int WindowsBuild = util::GetWinVersion();
        
        DWORD64 d2d1Module = reinterpret_cast<DWORD64>(GetModuleHandleA(("d2d1.dll")));
        DWORD64 dwmcoreModule = reinterpret_cast<DWORD64>(GetModuleHandleA(("dwmcore.dll")));

        if (WindowsBuild <= 19044)
        {
            DWORD64 CallMPO = util::FeatureFunc::Search(d2d1Module, XOR("48 ?? ?? 48 ?? ?? ?? ?? 8B ?? ?? ?? ?? ?? ?? 89 ?? ?? ?? E8"));
            DWORD64 CallDwm = util::FeatureFunc::Search(d2d1Module, XOR("48 8B ?? ?? ?? ?? ?? ?? 48 ?? ?? ?? ?? 8B ?? ?? ?? ?? ?? ?? 89 ?? ?? ?? 48 8B ?? ?? ?? ?? ?? ?? 48 ?? ?? ?? ?? E8"));

            if (CallMPO != 0)
            {
                CallMPO += 0x13;
                CallDwm += 0x25;
                oPresentMPO = reinterpret_cast<PresentMPO_>(ResolveRelative(CallMPO, 1, 5));
                oPresentDWM = reinterpret_cast<PresentDWM_>(ResolveRelative(CallDwm, 1, 5));
                util::CallReplace(d2d1Module, CallMPO, MyPresentMPO);
            }
        }

        if (WindowsBuild == 22000)
        {
            DWORD64 CallMPO_W11_21H2 = util::FeatureFunc::Search(dwmcoreModule, XOR("48 8B ?? ?? ?? ?? ?? ?? 48 ?? ?? ?? ?? 89 ?? ?? ?? E8 ?? ?? ?? ?? 8B"), 0x1000, 0xFFFFFFF);
            DWORD64 DwmCoreCallMPO = 0;

            if (CallMPO_W11_21H2)
                DwmCoreCallMPO = CallMPO_W11_21H2 + 0x11;

            if (DwmCoreCallMPO)
            {
                oPresentMPOW11 = reinterpret_cast<PresentMPO_W11>(ResolveRelative(DwmCoreCallMPO, 1, 5));
                util::CallReplace(dwmcoreModule, DwmCoreCallMPO, MyPresentMPO_W11);
            }
        }

        if (WindowsBuild >= 22621)
        {
            DWORD64 CallMPO_W11_22H2 = util::FeatureFunc::Search(dwmcoreModule, XOR("48 89 ?? ?? 48 C7 ?? ?? ?? ?? ?? ?? E8 ?? ?? ?? ?? 8B"), 0x1000, 0xFFFFFFF);
            DWORD64 DwmCoreCallMPO = 0;

            if (CallMPO_W11_22H2)
                DwmCoreCallMPO = CallMPO_W11_22H2 + 0xC;

            if (DwmCoreCallMPO)
            {
                oPresentMPOW11 = reinterpret_cast<PresentMPO_W11>(ResolveRelative(DwmCoreCallMPO, 1, 5));
                util::CallReplace(dwmcoreModule, DwmCoreCallMPO, MyPresentMPO_W11);
            }
        }
    }
}
