#pragma once

#include "glm/vec2.hpp"
#include <memory>

#include "Shape.h"

class RigidBody
{
public:

	RigidBody(const Shape& colliderShape, float x, float y, float mass);

	~RigidBody();

	RigidBody(const RigidBody& other) = delete;
	RigidBody(RigidBody&& other) = delete;
	RigidBody& operator=(const RigidBody& other) = delete;
	RigidBody& operator=(RigidBody&& other) = delete;

	void AddForce(const glm::vec2& force) { m_AccumulatedForce += force; }
	void AddTorque(const float torque) { m_AccumulatedTorque += torque; }

	void ClearForces() { m_AccumulatedForce = glm::vec2{}; }
	void ClearTorque() { m_AccumulatedTorque = 0; }

	void GenerateDrag(const float dragCoefficient);
	void GenerateFriction(const float frictionCoefficient);

	void Update(const float deltaTime);
	void Draw(SDL_Renderer* pRenderer) const;

	//Publically accesable linear motion variables
	glm::vec2 Pos{};
	glm::vec2 Velocity{};

	float Mass{};

	//Publically accesable angular motion variables
	float Rot{};

	float I{}; //Moment Of Inertia(Inertia)

private:

	//Pointer referencing the collider of the rigidbody
	std::unique_ptr<Shape> m_ColliderShape = nullptr;

	//Linear variables
	glm::vec2 m_AccumulatedForce{};
	glm::vec2 m_Acceleration{};

	float m_InvMass{};

	//Angular variables
	float m_AccumulatedTorque{};

	float m_AngularVelocity{};
	float m_AngularAcceleration{};

	float m_InvI{};

};

