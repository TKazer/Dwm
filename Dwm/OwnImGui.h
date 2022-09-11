#pragma once
#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx11.h"
#include <d3d11.h>
#pragma comment(lib,"D3D11.lib")

// 设备信息
extern IDXGISwapChain* g_pSwapChain;
extern ID3D11Device* g_pd3dDevice;
extern ID3D11DeviceContext* g_pd3dDeviceContext;
extern ID3D11RenderTargetView* g_mainRenderTargetView;
extern RECT ProgramRect;

namespace Gui
{
	void CalcPos(ImVec2& Pos);

	bool ImGuiInit(IDXGISwapChain* pSwapChain, ID3D11Device* pd3dDevice);

	void _DrawText(ImVec2 Pos, float Size, ImVec4 Color, const char* Text);
	void _DrawRect(ImVec2 Pos, float Width, float Height, ImVec4 Color, float Thickness);
	void _DrawLine(ImVec2 From, ImVec2 To, ImVec4 Color, float Thickness);
	void _DrawCircle(ImVec2 Pos, float Radius, ImVec4 Color, float Thickness);
	void _DrawRectFilled(ImVec2 Pos, float Width, float Height, ImVec4 Color, float Rounding);
}

