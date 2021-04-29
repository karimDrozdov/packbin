#pragma once
#include <Windows.h>
#include <iostream>
#include <string>

#include "physfs.h"

using namespace std;

class VFS
{
public:
	VFS();
	VFS(string dir);
	~VFS();

	void AddDir(string dir, string mask = "*.*");
	void AddFile(string fname);

	BOOL FileExists(string fname);
	long GetFileSize(string fname);
};

