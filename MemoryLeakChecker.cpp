#include <iostream>
#include "MemoryLeakChecker.h"

MemoryLeakChecker::MemoryLeakChecker() {
#ifdef _DEBUG
	_CrtMemCheckpoint(&sOld); //take a snapshot
#endif
}

MemoryLeakChecker::~MemoryLeakChecker() {
#ifdef _DEBUG
	_CrtMemState sNew;
	_CrtMemState sDiff;

	_CrtMemCheckpoint(&sNew); //take a snapshot 
	if (_CrtMemDifference(&sDiff, &sOld, &sNew)) // if there is a difference
	{

		std::cerr << "\nMemory leaks detected!\n" << std::endl;

		OutputDebugString(L"-----------_CrtMemDumpStatistics ---------");
		_CrtMemDumpStatistics(&sDiff);
		OutputDebugString(L"-----------_CrtMemDumpAllObjectsSince ---------");
		_CrtMemDumpAllObjectsSince(&sOld);
		OutputDebugString(L"-----------_CrtDumpMemoryLeaks ---------");
		_CrtDumpMemoryLeaks();
	}
	else {
		std::cerr << "\nNo memory leaks detected\n" << std::endl;
	}
#endif
}