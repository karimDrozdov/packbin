#pragma once
#include "Functional.h"

vector<string> files;

void Print()
{
	while (true)
	{
		if (files.size() > 0)
		{
			for (int i = 0; i < files.size(); i++)
			{
				cout << files[i] << endl;
				files.erase(files.begin() + i);
			}
		}
		Sleep(100);
	}
}

bool isPatched = false;
bool isPause = false;
HANDLE(WINAPI *CrFileA)(
	_In_ LPCSTR lpFileName,
	_In_ DWORD dwDesiredAccess,
	_In_ DWORD dwShareMode,
	_In_opt_ LPSECURITY_ATTRIBUTES lpSecurityAttributes,
	_In_ DWORD dwCreationDisposition,
	_In_ DWORD dwFlagsAndAttributes,
	_In_opt_ HANDLE hTemplateFile) = CreateFileA;

HANDLE WINAPI NewCrFileA(
	_In_ LPCSTR lpFileName,
	_In_ DWORD dwDesiredAccess,
	_In_ DWORD dwShareMode,
	_In_opt_ LPSECURITY_ATTRIBUTES lpSecurityAttributes,
	_In_ DWORD dwCreationDisposition,
	_In_ DWORD dwFlagsAndAttributes,
	_In_opt_ HANDLE hTemplateFile)
{
	if (!isPatched)
	{
		Patches();
//		StartRoutine(Print);
		isPatched = true;
	}
#if PRINT_BINARY
	string test = &lpFileName[strlen(lpFileName) - 3];
	if (test == "bin")
		cout << lpFileName << endl;
		//files.push_back(lpFileName);
#else 
#if PRINT_FILES
	cout << lpFileName << endl;
#endif
#endif

#if PAUSE
	if (!isPause)
	{
		system("pause");
		isPause = true;
	}
#endif
	return  CrFileA(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
}