#pragma once

#include "../glm/vec2.hpp"
#include <memory>

#include "Shape.h"

class RigidBody
{
public:

	RigidBody(const Shape& colliderShape, float x, float y, float mass, float restitution = 0.5f, float friction = 0.5f, float rot = 0.f);
	RigidBody(const Shape& colliderShape, int x, int y, float mass, float restitution = 0.5f, float friction = 0.5f, float rot = 0.f);

	~RigidBody();

	RigidBody(const RigidBody& other) = delete;
	RigidBody(RigidBody&& other) = delete;
	RigidBody& operator=(const RigidBody& other) = delete;
	RigidBody& operator=(RigidBody&& other) = delete;

	void AddForce(const glm::vec2& force) { m_AccumulatedForce += force; }
	void AddTorque(const float torque) { m_AccumulatedTorque += torque; }

	//Linear impulse
	void AddImpulse(const glm::vec2& impulse);

	//Rotational impulse
	void AddImpulse(const glm::vec2& impulse, const glm::vec2& dir);

	void ClearForces() { m_AccumulatedForce = glm::vec2{}; }
	void ClearTorque() { m_AccumulatedTorque = 0; }

	void Update(const float deltaTime);
	void Draw(SDL_Renderer* pRenderer) const;

	Shape* GetShape() const { return m_ColliderShape.get(); }

	//If mass is less than 0, object is static
	bool IsStatic() const { return InvMass <= 0.0005f; }

	//Publically accesable linear motion variables
	glm::vec2 Pos{};
	glm::vec2 Velocity{};

	float Mass{};
	float InvMass{};

	//Publically accesable angular motion variables
	float Rot{};
	float AngularVelocity{};

	float I{}; //Moment Of Inertia(Inertia)
	float InvI{};

	//Elasticity and friction
	float Elasticity{};
	float Friction{};

	bool colliding = false;

private:


	//Pointer referencing the collider of the rigidbody
	std::unique_ptr<Shape> m_ColliderShape = nullptr;

	//Linear variables
	glm::vec2 m_AccumulatedForce{};
	glm::vec2 m_Acceleration{};


	//Angular variables
	float m_AccumulatedTorque{};
	float m_AngularAcceleration{};


	static const float m_PI2;

	void EulerIntegration(const float deltaTime);
	void VerletIntegration(const float deltaTime);
	void RK4Integration(const float deltaTime);
};

