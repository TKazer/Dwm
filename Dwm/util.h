#pragma once
#include <Windows.h>
#include <iostream>
#include <Psapi.h>
#include <string>
#include <vector>
#include <Tlhelp32.h>
#include <winternl.h>
#include "xorstr.hpp"

namespace util
{
	template<typename T = void, typename... Arg>
	T Call(PVOID address, Arg... args)
	{
		return ((T(*)(Arg...))address)(args...);
	}

	namespace FeatureFunc
	{
		/*
			??通配符
			Demo:
				FeatureFunc::Find(Module,"48 E8 CC ?? 05 FF",0.0xffffffff)
		*/
		DWORD64 Search(DWORD64 Module, std::string Feature, DWORD64 StartOffset = 0, DWORD64 EndOffset = 0xFFFFFFFF);
	}

	namespace MapMem
	{
		typedef struct _Memory_func {
			PVOID pOpenFileMappingW;
			PVOID pMapViewOfFileExNuma;
		}Memory_func, * PMemory_func;

		BOOL GetMapMemFunction(PMemory_func func);
	}
	
	// Call指针替换
	void CallReplace(DWORD64 ModuleAddress, DWORD64 CallAddress, PVOID RepFunction);
	// 获取操作系统版本
	int GetWinVersion();
}