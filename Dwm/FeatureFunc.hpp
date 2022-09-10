#pragma once
#include <windows.h>
#include <string>
#include <vector>

namespace FeatureFunc
{
	/*
		??Í¨Åä·û
		Demo:
			FeatureFunc::Find(Module,"48 E8 CC ?? 05 FF",0.0xffffffff)
	*/
	DWORD64 Search(DWORD64 Module, std::string Feature, DWORD64 StartOffset=0, DWORD64 EndOffset=0xffffffff)
	{
		// È¥¿Õ
		for (int i = 0; i < Feature.length(); i++)
		{
			if (Feature[i] == ' ')
				Feature.replace(i, 1, "");
		}

		if (Feature.size() < 2)
			return 0;

		if (Feature.size() % 2 != 0)
			return 0;

		std::vector<std::pair<bool, byte>> Bytes;

		// ×Ö½Ú¡¢ÑÚÂëÉú³É
		for (int i = 0; i < Feature.length(); i += 2)
		{
			if (Feature.substr(i, 2) == "??")
				Bytes.push_back(std::make_pair<bool, byte>(false, 0x0));
			else
				Bytes.push_back(std::make_pair<bool, byte>(true, static_cast<byte>(std::stol(Feature.substr(i, 2), 0, 16))));
		}

		// ËÑË÷Æ¥Åä
		for (DWORD64 Offset = StartOffset; Offset < EndOffset; Offset++)
		{
			if (!IsBadHugeReadPtr(reinterpret_cast<PVOID>(Module + Offset), 1))
			{
				byte* pTempByte = reinterpret_cast<byte*>(Module + Offset);
				int ProofNum = 0;

				for (auto SingleByte : Bytes)
				{
					if (SingleByte.first && SingleByte.second != *pTempByte)
						break;
					pTempByte++;
					ProofNum++;
				}

				if (ProofNum == Bytes.size() && *pTempByte)
					return Module + Offset;
			}
		}
		return NULL;
	}
}