#pragma once
#include "../glm/vec2.hpp"

class Polygon;
class RigidBody;

class CollisionSolver final
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

	struct CollisionDataPoly
	{
		glm::vec2 seperationAxis{};
		glm::vec2 projectedPoint{};
		float seperationLength{};
	};

	static bool IsColliding(RigidBody* a, RigidBody* b, CollisionData& data);

	static float Cross(const glm::vec2& a, const glm::vec2& b) { return (a.x * b.y) - (a.y * b.x); }

private:

	static bool CircleVSCircle(RigidBody* a, RigidBody* b, CollisionData& data, bool detectOnly);

	static bool PolyVSPoly(RigidBody* a, RigidBody* b, CollisionData& data);

	static bool PolyVsCircle(RigidBody* polyBody, RigidBody* circleBody, CollisionData& data);

	static void PositionalCorrection(const CollisionData& data);

	static void AddImpulses(const CollisionData& data);

	static void FindLeastSeperation(const Polygon* a, const Polygon* b, CollisionDataPoly& data);
};

