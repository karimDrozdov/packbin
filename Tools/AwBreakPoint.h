#include <windows.h>
#include <tlhelp32.h>

typedef void(__cdecl* EXCEPTION_HANDLER)(EXCEPTION_POINTERS*);

class AwBreakPoint
{
public:

	bool SetExceptionHandler(EXCEPTION_HANDLER);
	bool SetHWBreakPoint(unsigned long, int);
	bool ClearHWBreakPoint(int);

	static EXCEPTION_HANDLER GetHandler();

private:

	void GetMainThreadFromProcessId();

	HANDLE m_hThread;
	static EXCEPTION_HANDLER Handler;

};