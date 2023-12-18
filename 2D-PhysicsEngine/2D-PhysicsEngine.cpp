#include <memory>

#include "PhysicsEngine.h"

int wmain()
{
	const std::unique_ptr<PhysicsEngine> engine = std::make_unique<PhysicsEngine>(800, 600, 0.02f);
	
    engine->Run();

	return 0;
}
