#include "Com.h"

void CMainCom::Com()
{
	if (!this->DrawData->ServerMutex)
		return;

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	for (int i = 0; i < this->DrawData->DrawCount; i++)
	{
		switch (this->DrawData->DrawDataArray[i].Type)
		{
		case TYPE_TEXT:
		{
			_Text& Temp = this->DrawData->DrawDataArray[i]._From.Text;
			Gui::_DrawText(Temp.Pos, Temp.Size, Temp.Color, Temp.Text);
		}
		break;
		case TYPE_RECT:
		{
			_Rect& Temp = this->DrawData->DrawDataArray[i]._From.Rect;
			Gui::_DrawRect(Temp.Min, Temp.Max, Temp.Color, Temp.Thickness);
		}
		break;
		case TYPE_LINE:
		{
			_Line& Temp = this->DrawData->DrawDataArray[i]._From.Line;
			Gui::_DrawLine(Temp.From, Temp.To, Temp.Color, Temp.Thickness);
		}
		break;
		case TYPE_CIRCLE:
		{
			_Circle& Temp = this->DrawData->DrawDataArray[i]._From.Circle;
			Gui::_DrawCircle(Temp.Pos, Temp.Radius, Temp.Color, Temp.Thickness);
		}
		break;
		case TYPE_RECTFILLED:
		{
			_RectFilled& Temp = this->DrawData->DrawDataArray[i]._From.RectFilled;
			Gui::_DrawRectFilled(Temp.Min, Temp.Max, Temp.Color, Temp.Rounding);
		}
		break;
		default:
			break;
		}
	}

	this->DrawData->DrawCount = 0;
	this->DrawData->ServerMutex = false;
	this->DrawData->ClientMutex = true;

	g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, NULL);
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

bool CMainCom::InitImGui(IDXGISwapChain* pSwapChain)
{
	ID3D11Device* pDevice = NULL;
	pSwapChain->GetDevice(__uuidof(ID3D11Device), (void**)&pDevice);
	return Gui::ImGuiInit(pSwapChain, pDevice);
}

bool CMainCom::InitMessageMap(PCWSTR MapName)
{
	util::MapMem::PMemory_func Func = new util::MapMem::Memory_func;
	if (!util::MapMem::GetMapMemFunction(Func))
		return false;
	HANDLE hMap = util::Call<HANDLE>(Func->pOpenFileMappingW, FILE_MAP_WRITE | FILE_MAP_READ, FALSE, MapName);
	if (!hMap) 
		return false;
	this->DrawData = (PDRAWLIST)util::Call<LPVOID>(Func->pMapViewOfFileExNuma, hMap, FILE_MAP_ALL_ACCESS, 0, 0, 0, 0, 0xFFFFFFFF);
	if (!this->DrawData) {
		CloseHandle(hMap);
		return false;
	}
	return true;
}