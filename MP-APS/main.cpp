#ifdef _DEBUG
// CRT Memory Leak detection
	#define _CRTDBG_MAP_ALLOC  
	#include <stdlib.h>  
	#include <crtdbg.h>
#endif
#include "Engine.h"

/***********************************************************************************/
int main() {
#ifdef _DEBUG
	// Detects memory leaks upon program exit
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	Engine engine(1280, 720);

	engine.Execute();

	return 0;
}
