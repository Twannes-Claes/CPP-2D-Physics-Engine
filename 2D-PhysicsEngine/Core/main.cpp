#include "PhysicsEngine.h"

int main()
{
	const std::unique_ptr<PhysicsEngine> engine = std::make_unique<PhysicsEngine>(800, 600, 0.0167f);
	//const std::unique_ptr<PhysicsEngine> engine = std::make_unique<PhysicsEngine>(1920, 1000, 0.0167f);
	
    engine->Run();

	return 0;
}