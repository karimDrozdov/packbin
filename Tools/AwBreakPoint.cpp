#include "AwBreakPoint.h"

EXCEPTION_HANDLER AwBreakPoint::Handler;

LONG __stdcall Ex_handler(EXCEPTION_POINTERS* ep)
{
	if (ep->ExceptionRecord->ExceptionCode == EXCEPTION_SINGLE_STEP)
	{
		EXCEPTION_HANDLER handler = AwBreakPoint::GetHandler();

		handler(ep);

		return EXCEPTION_CONTINUE_EXECUTION;
	}
	return EXCEPTION_CONTINUE_SEARCH;
}

void AwBreakPoint::GetMainThreadFromProcessId()
{
	unsigned long uProcessId = GetCurrentProcessId();
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, uProcessId);

	if (!hSnapshot)
		return;

	THREADENTRY32 lpThread;

	lpThread.dwSize = sizeof(THREADENTRY32);

	if (Thread32First(hSnapshot, &lpThread))
	{
		do {
			if (lpThread.th32OwnerProcessID == uProcessId)
			{
				break;
			}
		} while (Thread32Next(hSnapshot, &lpThread));

		CloseHandle(hSnapshot);
		AwBreakPoint::m_hThread = OpenThread(THREAD_GET_CONTEXT | THREAD_SET_CONTEXT | THREAD_SUSPEND_RESUME, 1, lpThread.th32ThreadID);
	}
	return;
}

bool AwBreakPoint::SetExceptionHandler(EXCEPTION_HANDLER eHandler)
{
	AwBreakPoint::Handler = eHandler;
	AddVectoredExceptionHandler(1, Ex_handler);
	return true;
}

EXCEPTION_HANDLER AwBreakPoint::GetHandler()
{
	return AwBreakPoint::Handler;
}

bool AwBreakPoint::SetHWBreakPoint(unsigned long uAddress, int iIndex)
{
	GetMainThreadFromProcessId();

	CONTEXT c = { CONTEXT_DEBUG_REGISTERS };

	SuspendThread(AwBreakPoint::m_hThread);

	GetThreadContext(AwBreakPoint::m_hThread, &c);

	switch (iIndex)
	{
	case 0:

		c.Dr0 = uAddress;
		c.Dr7 = (1 << 0); // set 0th bit
		break;

	case 1:

		c.Dr1 = uAddress;
		c.Dr7 |= (1 << 2); // set 2nd bit
		break;

	case 2:

		c.Dr2 = uAddress;
		c.Dr7 |= (1 << 4); // set 4th bit
		break;

	case 3:

		c.Dr3 = uAddress;
		c.Dr7 |= (1 << 6); // set 6th bit
		break;

	default:
		return false;
	}

	c.Dr6 = 0;

	SetThreadContext(AwBreakPoint::m_hThread, &c);

	ResumeThread(AwBreakPoint::m_hThread);

	return true;
}

bool AwBreakPoint::ClearHWBreakPoint(int iIndex)
{
	GetMainThreadFromProcessId();

	CONTEXT c = { CONTEXT_DEBUG_REGISTERS };

	SuspendThread(AwBreakPoint::m_hThread);

	GetThreadContext(AwBreakPoint::m_hThread, &c);

	switch (iIndex)
	{
	case 0:

		c.Dr7 = (0 << 0); // set 0th bit
		break;

	case 1:

		c.Dr7 |= (0 << 2); // set 2nd bit
		break;

	case 2:

		c.Dr7 |= (0 << 4); // set 4th bit
		break;

	case 3:

		c.Dr7 |= (0 << 6); // set 6th bit
		break;

	default:
		return false;
	}

	c.Dr6 = 0;

	SetThreadContext(AwBreakPoint::m_hThread, &c);

	ResumeThread(AwBreakPoint::m_hThread);

	return true;
}