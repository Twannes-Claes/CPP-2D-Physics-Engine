#include "RigidBody.h"

#pragma warning(push)
#pragma warning(disable: 4201) // Disable warning C4201
#include "glm/gtx/norm.hpp"
#pragma warning(pop)

RigidBody::RigidBody(float x, float y, float mass):
 pos(x, y),
 mass(mass)
{
	//Get the inverse of the mass
	invMass = 0;

	if (mass > 0) invMass = 1 / mass;
}

void RigidBody::AddForce(const glm::vec2& force)
{
	totalForce += force;
}

void RigidBody::GenerateDrag(const float dragCoefficient)
{
	//Fd = k * ||v||^2 * -Vn
	AddForce(dragCoefficient * glm::length2(v) * glm::normalize(v * -1.f));
}

void RigidBody::GenerateFriction(const float frictionCoefficient)
{
	//Ff = k * -Vn
	AddForce(frictionCoefficient * glm::normalize(v * -1.f));
}

void RigidBody::Update(const float deltaTime)
{
	//Euler integration of acceleration
	a = totalForce * invMass;

	v += a * deltaTime;

	pos += v * deltaTime;

	totalForce = glm::vec2{};
}
