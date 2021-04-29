#include "Maps.h"

extern BP br;
extern Patcher patch;
extern VFS vfs;

DWORD MapsBin = 0x00463520;
DWORD LoadMapBin(LPVOID dest, char * map_bin, LPVOID pack_bin_struct) //Return pBinary
{
	__asm
	{
		push[pack_bin_struct]
		push 0x0131DD02
		push[map_bin]
		mov ecx, dest
		call[MapsBin]
	}
}

DWORD LoadMapAddr = 0x00466F26;
DWORD LoadMapOff = 0x00466F28;

void __declspec(naked) LoadMaps(void) {
	__asm {
		cmp esi, ebx
		jmp[LoadMapOff]
	}
}


LPVOID Map = NULL;
void LoadMap(PCONTEXT db)
{
	isFirstLoad = false;
	if (Map == NULL)
	{
		Map = malloc(128);
		memset(Map, 0x0, 128);
	}
	string notBin = (char*)db->Esi;
	int amper = notBin.find("\\");
	if (amper != string::npos)
	{
		notBin[amper] = '_';
	}
	else
	{
		amper = notBin.find("//");
		if (amper != string::npos)
		{
			notBin[amper] = '_';
		}
	}

	for (int i = 0; i < bug_maps.size(); i++)
	{
		if (bug_maps[i] == notBin)
		{
			cout << "==========================" << endl;
			cout << notBin << " - BUGGED" << endl;
			cout << "==========================" << endl;
			bug_maps.erase(bug_maps.begin() + i);
			return;
		}
	}
	cout << "Map-Repack : " << notBin << endl;
	char buf[1024] = { 0 };
	strcpy(buf, BinMapsFolder.c_str()); strcat(buf, "/"); strcat(buf, notBin.c_str()); strcat(buf, ".bin");
	DWORD pMap = LoadMapBin(Map, buf, Pack);
}

void EnableMaps()
{
#if LOAD_MAPS_BIN
	br.AddINT3Break(LoadMapAddr, &LoadMaps, LoadMap);
#endif
}