#ifdef _DEBUG
// CRT Memory Leak detection
	#define _CRTDBG_MAP_ALLOC  
	#include <stdlib.h>  
	#include <crtdbg.h>
#endif
#include "Engine.h"

#include "Demos/DemoCrytekSponza.h"

/***********************************************************************************/
int main() {
#ifdef _DEBUG
	// Detects memory leaks upon program exit
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	Engine engine("Data/config.xml");

	const auto scene = std::make_shared<DemoCrytekSponza>();
	scene->Init("Sponza");

	engine.AddScene(std::static_pointer_cast<SceneBase, DemoCrytekSponza>(scene));
	engine.SetActiveScene("Sponza");

	engine.Execute();

	return 0;
}
