#ifdef _DEBUG
// CRT Memory Leak detection
	#define _CRTDBG_MAP_ALLOC  
	#include <stdlib.h>  
	#include <crtdbg.h>
#endif
#include "Engine.h"
#include <iostream>

/***********************************************************************************/
int main() {
#ifdef _DEBUG
	// Detects memory leaks upon program exit
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	Engine engine("Data/config.xml");

	try {
		engine.Execute();
	} 
	catch(const std::runtime_error& e) {
		std::cerr << e.what();
	} 
	catch(...) {
		std::cerr << "Unhandled exception!" << std::endl;
	}

	return 0;
}
