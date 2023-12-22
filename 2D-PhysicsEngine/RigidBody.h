#pragma once

#include "glm/vec2.hpp"
#include <memory>

#include "Shape.h"

class RigidBody
{
public:

	RigidBody(float x, float y, float mass);
	RigidBody(const Shape& colliderShape, float x, float y, float mass);

	~RigidBody();

	RigidBody(const RigidBody& other) = delete;
	RigidBody(RigidBody&& other) = delete;
	RigidBody& operator=(const RigidBody& other) = delete;
	RigidBody& operator=(RigidBody&& other) = delete;

	void AddForce(const glm::vec2& force);

	void GenerateDrag(const float dragCoefficient);
	void GenerateFriction(const float frictionCoefficient);

	void Update(const float deltaTime);
	void Draw(SDL_Renderer* pRenderer) const;

	//Position and Velocity can be accessed publically
	glm::vec2 pos{};
	glm::vec2 v{};

	float mass{};
	float invMass{};

private:

	std::unique_ptr<Shape> m_ColliderShape = nullptr;

	glm::vec2 a{};

	glm::vec2 accumulatedForce{};
};

