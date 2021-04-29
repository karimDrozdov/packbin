#include "Functional.h"
#include "Exceptions.h"

#include "Modules\Fixes.h"
#include "Modules\Main_Patches.h"
#include "Modules\Maps.h"
#include "Modules\Monitors.h"
#include "Modules\Patches.h"

bool isFirstLoad = true;
LPVOID Pack = malloc(128);

BP br;
Patcher patch;
VFS vfs;

void Patches()
{
	EnableFixes();
	EnableMaps();
	EnablePatches();
	EnableMain();
	EnableFilter();
}