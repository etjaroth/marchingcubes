#pragma once
#include "windows.h"
#define _CRTDBG_MAP_ALLOC //to get more details
#include <stdlib.h>  
#include <crtdbg.h>   //for malloc and free

class MemoryLeakChecker {

#ifdef _DEBUG
	_CrtMemState sOld;
#endif

public:
	MemoryLeakChecker();
	~MemoryLeakChecker();

};

