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

namespace Gui
{
	bool ImGuiInit(IDXGISwapChain* pSwapChain, ID3D11Device* pd3dDevice);

	void DrawText(ImVec2 Pos, float Size, ImVec4 Color, const char* Text);
	void DrawRect(ImVec2 Min, ImVec2 Max, ImVec4 Color, float Thickness);
	void DrawLine(ImVec2 From, ImVec2 To, ImVec4 Color, float Thickness);
	void DrawCircle(ImVec2 Pos, float Radius, ImVec4 Color, float Thickness);
	void DrawRectFilled(ImVec2 Min, ImVec2 Max, ImVec4 Color, float Rounding);
}

