#include "Main_Patches.h"

extern BP br;
extern Patcher patch;
extern VFS vfs;

void LoadPackBin()
{
	memset(Pack, 0, 128);
	X86Assembler a(&runtime);
	a.mov(edi, eax);
	a.push(0x0);
	a.push(0x01321108);
	a.push(imm_ptr(PackBin.data()));
	a.mov(ecx, imm_ptr(Pack));
	a.call(0x00463520);

	a.pusha();
	a.mov(ecx, imm_ptr(0x016DB388));
	a.mov(ecx, dword_ptr(ecx));
	a.mov(dword_ptr(ecx, 0x28), eax);
	a.popa();

	a.xor_(esi, esi);
	a.cmp(edi, esi);

	a.jmp(0x0046677F);

	patch.PlaceJit(0x00466779, &a, 1);
}
DWORD pOld = 0x0;
DWORD pRepl = 0x0;
vector<string> replacers;

void Replacer(DWORD struct_ori, DWORD struct_new)
{
	DWORD start = *(DWORD*)((DWORD)Pack + 0x20);
	DWORD end = *(DWORD*)((DWORD)Pack + 0x24);
	for (DWORD i = 0x0; start + i < end; i++)
	{
		DWORD zn = *(DWORD*)(start + i);
		if (zn == struct_ori)
			*(DWORD*)(start + i) = struct_new;
	}
}

bool StrCmp(char* str1, char* str2)
{
	for (int i = 0; str1[i] && str2[i]; i++)
	{
		if (str1[i] == '*' || str2[i] == '*')
			return true;
		if (str1[i] != str2[i])
			return false;
	}
	return true;
}

extern map<DWORD, int> reps;

bool FindReplacer(DWORD addr)
{
	return reps[addr];
	char * xdb = (char*)*(DWORD*)(addr + 0x0C);

	for (int i = 0; i < replacers.size(); i++)
	{
		char * str = (char*)replacers[i].c_str();
		if (StrCmp(str, xdb))
		{
			//printf("%s\n", xdb);
			//if(replacers[i].rfind('*') == string::npos)
			//	replacers.erase(replacers.begin() + i);
			return true;
		}
	}
	return false;
}

DWORD eipRep = 0x0043AF12;
_declspec(naked) void EipReplace()
{
	_asm
	{
		mov edi, eax
		jmp[eipRep]
	}
}

#include <io.h>
bool isFileExists(char* path)
{
	string fname = "../data/" + string(path);
	return access(fname.c_str(), 0) != -1;
}

vector<DWORD> builds;

void ReplaceForStruct(PCONTEXT db) // db->Eax = New struct | pOld = Game struct
{
	if (pOld == 0x0) return;

	DWORD check = *(DWORD*)(pOld + 0x10);
	if (check != 0)
	{
		return;
	}
	int zn = reps[pOld];
	if (zn && db->Eax != 0x0 && db->Eax != pOld)
	{
		DWORD ptr = *(DWORD*)pOld;

		static DWORD func_clear = 0x0;
		func_clear = *(DWORD*)(ptr + 0x14);
		DWORD(*getSize)() = reinterpret_cast<DWORD(*)(void)>(func_clear);

		static DWORD func = 0x0;
		static DWORD bak = 0x0;
		//static DWORD bak_bl = 0x0;

		func = *(DWORD*)(ptr);
		//bak_bl = *(DWORD*)0x0159B5C8;

		memset((void*)(pOld + 0x10), 0x0, getSize() - 0x10);

		__asm {
			pushad

			mov ecx, [pOld]
			mov ecx, [ecx + 0x0C]
			mov[bak], ecx

			mov ecx, [pOld]
			call func

			mov ecx, [pOld]
			lea ecx, [ecx + 0x0C]
			mov ebx, [bak]
			mov[ecx], ebx

			popad
		}
		*(DWORD*)0x0159B5C8 = 1;
		db->Eax = pOld;
		reps.erase(pOld);
	}
}

void EnableReplacer()
{
	br.AddINT3Break(0x0043AF10, &EipReplace, ReplaceForStruct);
}

void AltPatch() // FORCE LOADING HREF'S FORCON
{
	X86Assembler a(&runtime);
	a.mov(ecx, ebx);
	a.jmp(0x00A0F10F);
	patch.PlaceJit(0x00A0F109, &a, 1);
}

void Patch2()
{
	X86Assembler a(&runtime);
	Label xdb = a.newLabel();

	a.mov(ecx, dword_ptr(esi, 0x28));
	a.mov(eax, dword_ptr(ecx));
	a.mov(edx, dword_ptr(eax, 0x8));
	a.push(edi);
	a.call(edx); // Search in pack.bin
	a.test(eax, eax);
	a.je(xdb);
	a.cmp(dword_ptr(eax, 0x10), 0x0);
	a.jne(xdb);
	a.mov(eax, 0x0);

	a.bind(xdb);
	a.jmp(0x004397DE);

	patch.PlaceJit(0x004397D3, &a);
}

bool CheckBuild(DWORD addr)
{
	for(int i = 0; i < builds.size(); i++)
	{
		auto var = builds[i];
		if (addr == var)
			return true;
	}
	return false;
}

void Replace()
{
	X86Assembler a(&runtime);

	Label Null = a.newLabel();
	Label Exit = a.newLabel();
	Label Alt = a.newLabel();

#if REPLACE_FINDER
	AltPatch();
	a.mov(ptr_abs((DWORD)&pOld), eax);
#endif
	a.cmp(eax, 0x0);
	a.je(Null);

	a.cmp(dword_ptr(eax, 0x10), 0x0);
	a.jne(Exit);
	//a.mov(eax, 0x0);

	//a.pusha();
	//a.push(eax);
	//a.call((DWORD)&CheckBuild);
	//a.add(esp, 0x4);
	//a.cmp(eax, 1);
	//a.popa();
	//a.je(Exit);

	a.bind(Null);
	a.jmp(0x0043AEEA);

	a.bind(Exit);
	a.jmp(0x0043B0F4);

	patch.PlaceJit(0x0043AEE2, &a, 3);

	Patch2();
}

void Redirector()
{

	X86Assembler a(&runtime);

	Label notStruct = a.newLabel();
	Label Exit = a.newLabel();

	a.push(eax);
	a.mov(eax, ptr_abs((DWORD)&Map));
	a.cmp(eax, 0x0);
	a.pop(eax);
	a.je(notStruct);

	a.mov(ecx, ptr_abs(0x016DB388));

	a.push(eax);
	a.mov(eax, ptr_abs((DWORD)&Map));
	a.mov(dword_ptr(ecx, 0x28), eax);
	a.pop(eax);

	a.mov(eax, dword_ptr(ecx));
	a.mov(edx, dword_ptr(eax, 0x34));
	a.push(esi);
	a.call(edx);

	a.push(eax);
	a.mov(eax, ptr_abs((DWORD)&Pack));
	a.push(ecx);
	a.mov(ecx, ptr_abs(0x016DB388));
	a.mov(dword_ptr(ecx, 0x28), eax);
	a.pop(ecx);
	a.pop(eax);

	a.test(eax, eax);
	a.jne(Exit);

	a.bind(notStruct);
	a.mov(ecx, ptr_abs(0x016DB388));
	a.mov(eax, dword_ptr(ecx));
	a.mov(edx, dword_ptr(eax, 0x34));
	a.push(esi);
	a.call(edx);

	a.bind(Exit);
	a.jmp(0x00437C25);

	patch.PlaceJit(0x00437C17, &a, 1);
}
#pragma endregion

void DisableCheckFiles()
{
	X86Assembler a(&runtime);
	a.jmp(0x00F69DA5);
	patch.PlaceJit(0x00F69D64, &a);
}

void CutAbort()
{
	X86Assembler a(&runtime);
	a.ret();
	patch.PlaceJit((DWORD)GetProcAddress(GetModuleHandle("MSVCR100.dll"), "abort"), &a);
}

void EnableMain()
{
#if REPLACE_FINDER
	EnableReplacer();
#endif
	DisableCheckFiles();
	CutAbort();
#if LOAD_PACK_BIN
	LoadPackBin();
#endif
#if REPLACE_XDB
	Replace();
#endif
	Redirector();
	vfs.AddDir("../Data/Packs", "XDB_*.pak");
}