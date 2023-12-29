#pragma once
#include "glm/vec2.hpp"

class RigidBody;


class CollisionSolver
{
public:

	CollisionSolver() = delete;

	~CollisionSolver() = delete;

	CollisionSolver(const CollisionSolver& other) = delete;
	CollisionSolver(CollisionSolver&& other) = delete;
	CollisionSolver& operator=(const CollisionSolver& other) = delete;
	CollisionSolver& operator=(CollisionSolver&& other) = delete;

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
};

