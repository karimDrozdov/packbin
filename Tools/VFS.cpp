#include "VFS.h"

VFS::VFS()
{
	PHYSFS_init(NULL);
}

VFS::VFS(string dir)
{
	PHYSFS_init(NULL);
	AddDir(dir);
}


VFS::~VFS()
{
	PHYSFS_deinit();
}

void VFS::AddDir(string dir, string mask)
{
	WIN32_FIND_DATA fd; HANDLE handle;
	string strSpec = dir + "/" + mask;
	handle = FindFirstFile(strSpec.c_str(), &fd);
	if (handle == INVALID_HANDLE_VALUE)
		return;
	do {
		strSpec = fd.cFileName;
		if (strSpec != "." && strSpec != "..")
		{
			if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN))
			{
				strSpec = dir + "/" + strSpec;
				if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
					AddDir(strSpec);
				}
				else {
					if (fd.nFileSizeLow != 0 || fd.nFileSizeHigh != 0) {
						PHYSFS_addToSearchPath(strSpec.c_str(), 1);
					}
				}
			}
		}
	} while (FindNextFile(handle, &fd));
	FindClose(handle);
}

void VFS::AddFile(string fname)
{
	PHYSFS_addToSearchPath(fname.c_str(), 1);
}

BOOL VFS::FileExists(string fname)
{
	if (!PHYSFS_exists(fname.c_str())) return false;
	return true;
}

long VFS::GetFileSize(string fname)
{
	if (!PHYSFS_exists(fname.c_str())) return 0;
	PHYSFS_File* fileHandle = PHYSFS_openRead(fname.c_str());
	if(fileHandle != 0)
		return PHYSFS_fileLength(fileHandle);
	return -1;
}
