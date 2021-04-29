#include "Fixes.h"

extern BP br;
extern Patcher patch;
extern VFS vfs;

//===============FILTER======================//
int filter(unsigned int code, struct _EXCEPTION_POINTERS *ep) {
	cout << "=======================" << endl;
	if (code == EXCEPTION_ACCESS_VIOLATION) {
		cout << "ACCESS VIOLATION" << endl;
		return EXCEPTION_EXECUTE_HANDLER;
	}
	else {
		cout << "didn't catch AV, unexpected." << endl;
		return EXCEPTION_CONTINUE_SEARCH;
	};
}
//===============FILTER======================//

//=========Fix_Price===========//
DWORD price_fix = 0x00A6C551;
__declspec(naked) void price_fixer()
{
	__asm
	{
		push ebp
		jmp[price_fix]
	}
}
void Price_Fixer(PCONTEXT db)
{
	*(DWORD*)db->Ecx = 0x0;
}

//==========Fix_Texts_Source===========//
DWORD text_addr = 0x00580340;
DWORD text_fix = 0x00580341;
DWORD text_ret = 0x005803B2;
_declspec(naked) void text_fixer()
{
	__asm
	{
		push ebp
		jmp[text_fix]
	}
}
void Text_Fixer(PCONTEXT db)
{
	DWORD pXdb = *(DWORD*)db->Eax;
	__try
	{
		if (*(DWORD*)pXdb == NULL)
		{
			db->Eip = text_ret;
			return;
		}
	}
	__except (filter(GetExceptionCode(), GetExceptionInformation()))
	{
		printf("Exception address: %X\n", db->Eax);
		printf("Exception pointer: %X\n", pXdb);
		cout << "=======================" << endl;

		db->Eip = text_ret;
		*(DWORD*)db->Eax = 0x0;
		*(DWORD*)(db->Eax + 4) = 0x0;
		*(DWORD*)(db->Eax + 8) = 0x0;
		return;
	}
	db->Eip = (DWORD)&text_fixer;
}

void EnableFixes()
{
	br.AddEipINT3Break(text_addr, Text_Fixer);
	br.AddINT3Break(0x00A6C550, &price_fixer, Price_Fixer);
}