#include <memory>

#include "PhysicsEngine.h"

int main()
{
	const std::unique_ptr<PhysicsEngine> engine = std::make_unique<PhysicsEngine>(800, 600, 0.02f);

    engine->Run();
}
