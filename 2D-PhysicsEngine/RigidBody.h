#pragma once

#include "glm/vec2.hpp"
#include "glm/geometric.hpp"

class RigidBody
{

public:

	RigidBody() = default;
	~RigidBody() = default;

	RigidBody(const RigidBody& other) = delete;
	RigidBody(RigidBody&& other) = delete;
	RigidBody& operator=(const RigidBody& other) = delete;
	RigidBody& operator=(RigidBody&& other) = delete;

	void Update(float timeStep);

private:


	struct Transform
	{
		glm::vec2 pos{};

		float rotation{};
	};

	struct PhysicsMat
	{
		float density{};
		float restitution{};
	};

	struct Mass
	{
		float mass{};
		float invMass{};

		float inertia{};
		float invInertia{};
	};

	//Shape* m_pShape;

	Transform m_Transform;

	glm::vec2 m_Velocity;
	glm::vec2 m_Force;

	PhysicsMat m_Mat;
	Mass m_Mass;

	//float gravityScale;

};

