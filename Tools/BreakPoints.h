#pragma once
#include <Windows.h>
#include <vector>

class BreakPoints
{
public:
	BreakPoints();
	~BreakPoints();

	void AddAddr(DWORD addr, LPVOID pEip, int bp, LPVOID func = NULL, bool isEip = false);
	void ClearAddr(DWORD addr, int bp);
};

