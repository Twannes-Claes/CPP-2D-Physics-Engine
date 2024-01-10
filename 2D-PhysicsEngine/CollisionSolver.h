#pragma once
#include "glm/vec2.hpp"

class Polygon;
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

	struct CollisionDataPoly
	{
		glm::vec2 seperationAxis{};
		glm::vec2 projectedPoint{};
		float seperationLength{};
	};

	static bool IsColliding(RigidBody* a, RigidBody* b, CollisionData& data);


	inline static float Cross(const glm::vec2& a, const glm::vec2& b);

private:

	static bool CircleVSCircle(RigidBody* a, RigidBody* b, CollisionData& data, bool detectOnly);

	static bool PolyVSPoly(RigidBody* a, RigidBody* b, CollisionData& data);

	static bool CircleVsPoly(RigidBody* polyBody, RigidBody* circleBody, CollisionData& data);

	static void ProjectionMethod(const CollisionData& data);

	static void ImpulseMethod(const CollisionData& data);

	static void FindLeastSeperation(const Polygon* a, const Polygon* b, CollisionDataPoly& data);

};

