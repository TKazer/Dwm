#include "util.h"

void util::CallReplace(DWORD64 ModuleAddress, DWORD64 CallAddress, PVOID RepFunction)
{
	DWORD64 ShellCodeAddress = FeatureFunc::Search(ModuleAddress+0x1000, XOR("CC CC CC CC CC CC CC CC CC CC CC CC"));

	ULONG OldProtect = 0;
	VirtualProtect(reinterpret_cast<PVOID>(ShellCodeAddress), 12, PAGE_EXECUTE_READWRITE, &OldProtect);
	*reinterpret_cast<USHORT*>(ShellCodeAddress) = 0xB848;
	*reinterpret_cast<PVOID*>(ShellCodeAddress + 2) = RepFunction;
	*reinterpret_cast<USHORT*>(ShellCodeAddress + 10) = 0xE0FF;
	VirtualProtect(reinterpret_cast<PVOID>(ShellCodeAddress), 12, OldProtect, &OldProtect);

	VirtualProtect(reinterpret_cast<PVOID>(CallAddress), 5, PAGE_EXECUTE_READWRITE, &OldProtect);
	*reinterpret_cast<DWORD*>(CallAddress + 1) = (ShellCodeAddress - CallAddress - 5);
	VirtualProtect(reinterpret_cast<PVOID>(CallAddress), 5, OldProtect, &OldProtect);
}

int util::GetWinVersion()
{
	HKEY  hKey;
	const char* SubKey = XOR("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion");
	const char* MainVersion = XOR("CurrentBuild");

	DWORD dwType = REG_SZ;
	DWORD dwLen = MAX_PATH;
	char Buffer[MAX_PATH]{};

	RegOpenKeyA(HKEY_LOCAL_MACHINE, SubKey, &hKey);
	RegQueryValueExA(hKey, MainVersion, 0, &dwType, (LPBYTE)Buffer, &dwLen);
	RegCloseKey(hKey);

	return std::stoi(std::string(Buffer));
}