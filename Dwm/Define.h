#pragma once
#include "OwnImGui.h"

using PresentMPO_ = __int64(__fastcall*)(void*, IDXGISwapChain*, unsigned int, unsigned int, int, __int64, __int64, int);
using PresentDWM_ = __int64(__fastcall*)(void*, IDXGISwapChain*, unsigned int, unsigned int, const struct tagRECT*, unsigned int, const struct DXGI_SCROLL_RECT*, unsigned int, struct IDXGIResource*, unsigned int);
using PresentMPO_W11 = __int64(__fastcall*)(void*, IDXGISwapChain*);

namespace DwmEntry
{
	static PresentMPO_ oPresentMPO = NULL;
	static PresentDWM_ oPresentDWM = NULL;
	static PresentMPO_W11 oPresentMPOW11 = NULL;
}