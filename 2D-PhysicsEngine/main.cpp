#include <memory>

#include "PhysicsEngine.h"

int wmain()
{
	const std::unique_ptr<PhysicsEngine> engine1 = std::make_unique<PhysicsEngine>(800, 600, 0.02f);
	
    engine1->Run();

	//PhysicsEngine engine = { 800, 600, 0.02f };
	//
	//engine.Run();

	return 0;
}