#pragma once
#include <Windows.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <map>

using namespace std;

#include "Patcher.h"
#include "BreakPoints/BP.h"

#include "Root\Functional.h"
#include "Tools\VFS.h"

#define StartRoutine(Function) CreateThread(0,0,(LPTHREAD_START_ROUTINE)Function,0,0,0);