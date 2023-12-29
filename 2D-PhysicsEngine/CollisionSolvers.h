#pragma once
#include "RigidBody.h"

namespace CollisionSolver
{
	struct CollisionData
	{
		RigidBody* a{};
		RigidBody* b{};

		glm::vec2 start{};
		glm::vec2 end{};
		glm::vec2 normal{};

		float depth{};
	};

	static bool IsColliding(RigidBody* a, RigidBody* b, CollisionData& data);

	static bool CircleVSCircle(RigidBody* a, RigidBody* b, CollisionData& data);
}
