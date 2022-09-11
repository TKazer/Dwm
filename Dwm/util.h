#pragma once
#include <Windows.h>
#include <string>
#include <vector>

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
			??Í¨Åä·û
			Demo:
				FeatureFunc::Find(Module,"48 E8 CC ?? 05 FF",0.0xffffffff)
		*/
		DWORD64 Search(DWORD64 Module, std::string Feature, DWORD64 StartOffset = 0, DWORD64 EndOffset = 0xffffffff);
	}

	namespace MapMem
	{
		typedef struct _Memory_func {
			PVOID pOpenFileMappingW;
			PVOID pMapViewOfFileExNuma;
		}Memory_func, * PMemory_func;

		BOOL GetMapMemFunction(PMemory_func func);
	}
}