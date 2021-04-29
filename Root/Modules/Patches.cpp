#include "Patches.h"

extern BP br;
extern Patcher patch;
extern VFS vfs;

#include <io.h>
bool FileExists(const char *fname)
{
	return access(fname, 0) != -1;
}

DWORD GetXdbAddr = 0x00461BF5;
DWORD GetXdbOff = 0x00461BF9;

void __declspec(naked) GetXdb(void) {
	__asm {
		mov[esi + eax * 4 + 0x0C], ebx
		jmp[GetXdbOff]
	}
}

BOOL CrDir(LPCTSTR sPathTo)
{
	BOOL bRet = sPathTo ? TRUE : FALSE;

	while (bRet && (bRet = CreateDirectory(sPathTo, NULL)) == FALSE)
	{

		__try {
			int k = lstrlen(sPathTo);
			if (k >= MAX_PATH) break;
			LPTSTR   sTemp = LPTSTR(_alloca((k + 1) * sizeof(TCHAR)));
			lstrcpy(sTemp, sPathTo);
			while (k && sTemp[k] != TCHAR('\\') && sTemp[k] != TCHAR('/')) --k;
			if (k <= 0) break;
			sTemp[k] = TCHAR('\0');
			bRet = CrDir(sTemp);
		}
		__except (GetExceptionCode() == STATUS_STACK_OVERFLOW) { break; }
	}
	return bRet;
}
void CreateDir(string file)
{
	char * _path = new char[500];
	_splitpath(file.c_str(), NULL, _path, NULL, NULL);
	CrDir(_path);
	delete[] _path;
}


void CrFile(string fname, string dir)
{
	CreateDir(dir + "/" + fname);
	ofstream out(dir + "/" + fname);
	out << "<?xml version=\"1.0\" encoding=\"UTF - 8\" ?>" << endl;
	out.close();
}

vector<string> ignores;
bool StrCmp(char* str1, char* str2);
extern vector<string> replacers;

map<DWORD, int> reps;

void CheckXDBS(PCONTEXT db)
{
	if (!isFirstLoad) return;

	DWORD struct_a = db->Esi + db->Eax * 4;
	char * xdb = (char*)db->Ebx;
	if (xdb == NULL) return;

#if CREATE_FIX_EMPTY_MAPS
	if (isMap)
	{
		CrFile(xdb, "Empty");
	}
#endif
#if WORKER_FOR_REPLACE
	for (int i = 0; i < ignores.size(); i++)
	{
		char * ign = (char*)ignores[i].c_str();
		if (StrCmp(ign, xdb))
			return;
	}
	int flag = 0;
	for (int i = 0; i < replacers.size(); i++)
	{
		char * str = (char*)replacers[i].c_str();
		if (StrCmp(str, xdb))
		{
			flag = 1;
			break;
		}
	}
	reps.insert(pair<DWORD, int>(struct_a, flag));
	string fname = "../Data/" + string(xdb);
	if (FileExists(fname.c_str()))
	{
		*(DWORD*)(struct_a + 0x10) = 0x0;
	}
	else if (vfs.FileExists(xdb))
	{
		*(DWORD*)(struct_a + 0x10) = 0x0;
	}
#endif
}

void EnablePatches()
{
#if WORKER_FOR_REPLACE
	br.AddINT3Break(GetXdbAddr, &GetXdb, CheckXDBS);
#endif
}