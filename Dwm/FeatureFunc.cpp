#include "util.h"
#include <Psapi.h>
DWORD64 util::FeatureFunc::Search(DWORD64 Module, std::string Feature, DWORD64 StartOffset, DWORD64 EndOffset)
{
	// ȥ��
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

	// �ֽڡ���������
	for (int i = 0; i < Feature.length(); i += 2)
	{
		if (Feature.substr(i, 2) == "??")
			Bytes.push_back(std::make_pair<bool, byte>(false, 0x0));
		else
			Bytes.push_back(std::make_pair<bool, byte>(true, static_cast<byte>(std::stol(Feature.substr(i, 2), 0, 16))));
	}

	// ����ƥ��
	for (DWORD64 Offset = StartOffset; Offset < EndOffset; Offset++)
	{
		//if (!IsBadHugeReadPtr(reinterpret_cast<PVOID>(Module + Offset), 1))
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