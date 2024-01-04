#include <memory>

#include "PhysicsEngine.h"

int wmain()
{
	const std::unique_ptr<PhysicsEngine> engine1 = std::make_unique<PhysicsEngine>(800, 600, 0.0167f);
	
    engine1->Run();

	return 0;
}