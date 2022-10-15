#pragma once
#include <cstdio>

class Utils {
public:
	static VOID InitConsole()
	{
		AllocConsole();

		FILE* pFile;
		freopen_s(&pFile, "CONOUT$", "w", stdout);

	}
};