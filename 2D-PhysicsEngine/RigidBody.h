#pragma once

#include "glm/vec2.hpp"

class RigidBody
{
public:

	RigidBody(float x, float y, float mass);

	~RigidBody() = default;

	RigidBody(const RigidBody& other) = delete;
	RigidBody(RigidBody&& other) = delete;
	RigidBody& operator=(const RigidBody& other) = delete;
	RigidBody& operator=(RigidBody&& other) = delete;

	void AddForce(const glm::vec2& force);
	void GenerateDrag(const float dragCoefficient);
	void GenerateFriction(const float frictionCoefficient);

	void Update(const float deltaTime);

	glm::vec2 pos{};
	glm::vec2 v{};
	glm::vec2 a{};

	float mass{};
	float invMass{};

private:

	glm::vec2 totalForce{};
};

