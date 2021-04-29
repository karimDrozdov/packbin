#include "Exceptions.h"
#include <tlhelp32.h>
#include <psapi.h>

MODULEENTRY32 mod;

MODULEENTRY32  GetModuleInfo(char *name) {
	MODULEENTRY32 xModule;
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, GetCurrentProcessId());
	xModule.dwSize = sizeof(MODULEENTRY32);
	if (Module32First(hSnap, &xModule)) {
		while (Module32Next(hSnap, &xModule)) {
			if (!strncmp((char*)xModule.szModule, name, 8)) {
				CloseHandle(hSnap);
				return xModule;
			}
		}
	}
	CloseHandle(hSnap);
	return xModule;
}

void PrintRecords(PCONTEXT db)
{
	cout << "----------REGISTERS----------" << endl;
	printf("EAX: %X\n", db->Eax);
	printf("EBX: %X\n", db->Ebx);
	printf("ECX: %X\n", db->Ecx);
	printf("EDX: %X\n", db->Edx);
	printf("ESI: %X\n", db->Esi);
	printf("EDI: %X\n", db->Edi);
	printf("EBP: %X\n", db->Ebp);
	printf("ESP: %X\n", db->Esp);
	cout << "-----------------------------" << endl;
}

void PrintContext(EXCEPTION_POINTERS* ep)
{
	if (ep->ExceptionRecord->ExceptionCode == EXCEPTION_ACCESS_VIOLATION)
	{
		cout << "==========ACCESS_VIOLATION==========" << endl;
		printf("Address: %X\n", (DWORD)ep->ExceptionRecord->ExceptionAddress);
	}
}

bool DivByZero(EXCEPTION_POINTERS* ep);
bool Search(EXCEPTION_POINTERS* ep);
LONG WINAPI ExceptionHooker(EXCEPTION_POINTERS* ep)
{
	if (ep->ExceptionRecord->ExceptionCode == EXCEPTION_SINGLE_STEP || ep->ExceptionRecord->ExceptionCode == EXCEPTION_BREAKPOINT)
		return EXCEPTION_CONTINUE_SEARCH;

	DWORD addr = (DWORD)ep->ExceptionRecord->ExceptionAddress;

	if (ep->ExceptionRecord->ExceptionCode == EXCEPTION_ACCESS_VIOLATION)
	{
		if (addr >= (DWORD)mod.modBaseAddr && addr <= (DWORD)mod.modBaseAddr + mod.modBaseSize)
			return EXCEPTION_CONTINUE_SEARCH;
		if(Search(ep))
			return EXCEPTION_CONTINUE_EXECUTION;
		PrintContext(ep);
		PrintRecords(ep->ContextRecord);
		system("Pause");
		exit(0x0);
		return EXCEPTION_CONTINUE_EXECUTION;
	}
	else if (ep->ExceptionRecord->ExceptionCode == EXCEPTION_INT_DIVIDE_BY_ZERO)
	{
		if (DivByZero(ep))
			return EXCEPTION_CONTINUE_EXECUTION;
		cout << "==========DIVISION_BY_ZERO==========" << endl;
		printf("Address: %X\n", (DWORD)ep->ExceptionRecord->ExceptionAddress);
		PrintRecords(ep->ContextRecord);
		system("Pause");
		exit(0x0);
	}
	return EXCEPTION_CONTINUE_SEARCH;
}

void EnableFilter()
{
	//memset(&mod, 0, sizeof(mod));
	mod = GetModuleInfo("PackBin.dll");

	//printf("%X\n", (DWORD)mod.modBaseAddr);
	//printf("%X\n", (DWORD)mod.modBaseAddr + mod.modBaseSize);

	AddVectoredExceptionHandler(0, ExceptionHooker);
}

bool Search(EXCEPTION_POINTERS* ep)
{
	DWORD addr = (DWORD)ep->ExceptionRecord->ExceptionAddress;
	if (addr == 0x00580358) // String Exception
	{
		cout << "==================================" << endl;
		cout << "STRING EXCEPTION" << endl;
		printf("ADDRESS: %X\n", ep->ContextRecord->Eax);
		cout << "==================================" << endl;
		ep->ContextRecord->Eip = (DWORD)0x005803AC;
		return true;
	}
	//else if (addr = 0x465F79)
	//{
	//	cout << "==================================" << endl;
	//	cout << "STRING EXCEPTION 2" << endl;
	//	printf("ADDRESS: %X\n", ep->ContextRecord->Eax);
	//	cout << "==================================" << endl;
	//	ep->ContextRecord->Eip = (DWORD)0x00465FD3;
	//	return true;
	//}
	//else if (addr = 0x0043BA20)
	//{
	//		cout << "==================================" << endl;
	//		cout << "CONSTRUCTOR EXCEPTION" << endl;
	//		printf("ADDRESS: %X\n", ep->ContextRecord->Eax);
	//		cout << "==================================" << endl;
	//		ep->ContextRecord->Eax = 0x0;//ep->ContextRecord->Ecx;
	//		ep->ContextRecord->Eip = (DWORD)0x0043BA23;
	//		return true;
	//}
	return false;
}

bool DivByZero(EXCEPTION_POINTERS* ep)
{
	DWORD addr = (DWORD)ep->ExceptionRecord->ExceptionAddress;
	if (addr == 0x00465167) // String Exception
	{
		cout << "==================================" << endl;
		cout << "DIV BY ZERO" << endl;
		printf("ADDRESS: %X\n", ep->ContextRecord->Eax);
		cout << "==================================" << endl;
		ep->ContextRecord->Eip = (DWORD)0x00465169;
		return true;
	}
}