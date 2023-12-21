#include "RigidBody.h"

#pragma warning(push)
#pragma warning(disable: 4201) // Disable warning C4201
#include "glm/gtx/norm.hpp"
#pragma warning(pop)


RigidBody::RigidBody(const float x, const float y, float const mass) :
	pos(x, y),
	mass(mass)
{
	//Get the inverse of the mass
	invMass = 0;

	//Check if mass is not zero
	if (mass > 0) invMass = 1 / mass;
}

RigidBody::RigidBody(const Shape& colliderShape, const float x, const float y, float const mass):
 pos(x, y),
 mass(mass)
{
	//Implemented prototype pattern for easy transfer of shapes
	//No need to std::make_unique<ShapeType>(all the arguments)
	//Now every shape class has a function clone that returns an unique pointer with its own arguments
	//Learned this in programming 4 https://gameprogrammingpatterns.com/prototype.html
	m_ColliderShape = colliderShape.Clone();

	//Get the inverse of the mass
	invMass = 0;
	
	if (mass > 0) invMass = 1 / mass;
}

RigidBody::~RigidBody() = default;

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

void RigidBody::Draw(SDL_Renderer* pRenderer) const
{
	pRenderer;
}
