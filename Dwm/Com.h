#pragma once
#include "OwnImGui.h"
#include "DrawDataStruct.h"
#include "util.h"

class CMainCom
{
private:
	PDRAWLIST DrawData = nullptr;
public:
	CMainCom() {};
	void Com();
	bool InitImGui(IDXGISwapChain* pSwapChain);
	bool InitMessageMap(PCWSTR MapName);
};