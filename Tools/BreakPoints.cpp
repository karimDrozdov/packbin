#include "BreakPoints.h"
#include "AwBreakPoint.h"

using namespace std;

struct Break 
{
	DWORD addr;
	DWORD moreActions;
	LPVOID eip;
	bool isEip;
};

vector<Break> breaks;
AwBreakPoint BP;

void Handle(EXCEPTION_POINTERS* ep)
{
	for (int i = 0; i < breaks.size(); i++)
	{
		if (ep->ContextRecord->Eip == breaks[i].addr)
		{
			if (breaks[i].isEip)
			{
				if (breaks[i].moreActions != NULL)
				{
					void(*pFunction)(PCONTEXT);
					(FARPROC &)pFunction = (FARPROC)breaks[i].moreActions;
					pFunction(ep->ContextRecord);
				}
			}
			else
			{
				if (breaks[i].moreActions != NULL)
				{
					void(*pFunction)(PCONTEXT);
					(FARPROC &)pFunction = (FARPROC)breaks[i].moreActions;
					pFunction(ep->ContextRecord);
				}
				ep->ContextRecord->Eip = (DWORD)breaks[i].eip;
			}	
		}
	}
}

BreakPoints::BreakPoints()
{
	BP.SetExceptionHandler(Handle);
}


BreakPoints::~BreakPoints()
{
}

void BreakPoints::AddAddr(DWORD addr, LPVOID pEip, int bp, LPVOID func, bool isEip)
{
	Break b;
	b.addr = addr;
	b.eip = pEip;
	b.moreActions = NULL;
	if (func != NULL)
		b.moreActions = (DWORD)func;
	b.isEip = isEip;
	if (!BP.SetHWBreakPoint(addr, bp))
		printf("BP not hooked!\n");
	breaks.push_back(b);
}

void BreakPoints::ClearAddr(DWORD addr, int bp)
{
	for (int i = 0; i < breaks.size(); i++)
	{
		if (breaks[i].addr == addr)
		{
			BP.ClearHWBreakPoint(bp);
			breaks.erase(breaks.begin() + i);
			return;
		}
	}
}
