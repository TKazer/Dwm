#include "OwnImGui.h"

IDXGISwapChain* g_pSwapChain;
ID3D11Device* g_pd3dDevice;
ID3D11DeviceContext* g_pd3dDeviceContext;
ID3D11RenderTargetView* g_mainRenderTargetView;
ImFont* Font;

bool Gui::ImGuiInit(IDXGISwapChain* pSwapChain, ID3D11Device* pd3dDevice)
{
	g_pSwapChain = pSwapChain;
	g_pd3dDevice = pd3dDevice;
	g_pd3dDevice->GetImmediateContext(&g_pd3dDeviceContext);

	// 创建渲染目标
	ID3D11Texture2D* p_Buffer;
	g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&p_Buffer));
	if (p_Buffer == nullptr)
		return false;
	g_pd3dDevice->CreateRenderTargetView(p_Buffer, NULL, &g_mainRenderTargetView);
	p_Buffer->Release();

	// 创建imgui上下文
	ImGui::CreateContext();

	// 初始化ImGui
	ImGui_ImplWin32_Init(FindWindow(L"Progman", L"Program Manager"));
	ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);

	// 加载字体
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.IniFilename = nullptr;
	io.LogFilename = nullptr;
	Font = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\msyh.ttc", 16, 0, io.Fonts->GetGlyphRangesChineseFull());
	return true;
}

void Gui::_DrawText(ImVec2 Pos,float Size, ImVec4 Color, const char* Text)
{
	ImGui::GetForegroundDrawList()->AddText(Font, Size, Pos, ImGui::ColorConvertFloat4ToU32(Color), Text);
}

void Gui::_DrawRectFilled(ImVec2 Min, ImVec2 Max, ImVec4 Color, float Rounding)
{
	ImGui::GetForegroundDrawList()->AddRectFilled(Min, Max, ImGui::ColorConvertFloat4ToU32(Color), Rounding);
}

void Gui::_DrawRect(ImVec2 Min, ImVec2 Max, ImVec4 Color, float Thickness)
{
	ImGui::GetForegroundDrawList()->AddRect(Min, Max, ImGui::ColorConvertFloat4ToU32(Color), 0, 0, Thickness);
}

void Gui::_DrawLine(ImVec2 From, ImVec2 To, ImVec4 Color, float Thickness)
{
	ImGui::GetForegroundDrawList()->AddLine(From, To, ImGui::ColorConvertFloat4ToU32(Color), Thickness);
}

void Gui::_DrawCircle(ImVec2 Pos, float Radius, ImVec4 Color, float Thickness)
{
	ImGui::GetForegroundDrawList()->AddCircle(Pos, Radius, ImGui::ColorConvertFloat4ToU32(Color), (Radius > 50 ? Radius : 50), Thickness);
}