#pragma once
#include "Global.h"

//CONSOLE
#define SHOW_CONSOLE 1
#define PAUSE 1

//PRINT OPERATIONS
#define PRINT_FILES 0
#define PRINT_BINARY 0

//BINARY OPERATIONS
#define LOAD_MAPS_BIN 1
#define LOAD_PACK_BIN 1

//REPLACING OPERATIONS
#define REPLACE_FINDER 1 // Do not recommended because error packing
#define REPLACE_XDB 1
#define WORKER_FOR_REPLACE 1

//NULL TEXTURES OF MAPS FOR FIX
#define CREATE_FIX_EMPTY_MAPS 0 //Temporary not working

void Manager();
void ReadSettings(string fname);

extern string PackBin;
extern string BinMapsFolder;
extern vector<string> bug_maps;

extern LPVOID Pack;
extern LPVOID Map;

extern bool isFirstLoad;