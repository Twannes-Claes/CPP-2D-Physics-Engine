#include "RigidBody.h"

#pragma warning(push)
#pragma warning(disable: 4201) // Disable warning C4201
#include "glm/gtx/norm.hpp"
#pragma warning(pop)

RigidBody::RigidBody(const Shape& colliderShape, const float x, const float y, float const mass):
 Pos(x, y),
 Mass(mass)
{
	//Implemented prototype pattern for easy transfer of shapes
	//No need to std::make_unique<ShapeType>(all the arguments)
	//Now every shape class has a function clone that returns an unique pointer with its own arguments
	//Learned this in programming 4 https://gameprogrammingpatterns.com/prototype.html

	//Get the inverse of the mass
	m_InvMass = 0;
	
	if (mass > 0) m_InvMass = 1 / mass;

	//Get the unique pointer cloned by the Shape
	m_ColliderShape = colliderShape.Clone();

	I = m_ColliderShape->GetMomentOfInteria(mass);

	m_InvI = 0;

	if (I > 0.f) m_InvI = 1 / I;
}

RigidBody::~RigidBody() = default;

void RigidBody::GenerateDrag(const float dragCoefficient)
{
	//Fd = k * ||v||^2 * -Vn
	const float length2 = glm::length2(Velocity);

	if (length2 > 0) AddForce(dragCoefficient * length2 * glm::normalize(Velocity * -1.f));
}

void RigidBody::GenerateFriction(const float frictionCoefficient)
{
	//Ff = k * -Vn
	AddForce(frictionCoefficient * glm::normalize(Velocity * -1.f));
}

void RigidBody::Update(const float deltaTime)
{
	//Euler integration of acceleration
	m_Acceleration = m_AccumulatedForce * m_InvMass;

	Velocity += m_Acceleration * deltaTime;

	Pos += Velocity * deltaTime;

	//Clear linear forces
	m_AccumulatedForce = glm::vec2{};

	//Euler integration of rotation acceleration
	m_AngularAcceleration = m_AccumulatedTorque * m_InvI;

	m_AngularVelocity += m_AngularAcceleration * deltaTime;

	Rot += m_AngularVelocity * deltaTime;

	Rot = std::fmodf(Rot, 360.f);

	//Clear torque force
	m_AccumulatedTorque = 0;
}

void RigidBody::Draw(SDL_Renderer* pRenderer) const
{
	//Update rot of the shape when drawing
	m_ColliderShape->UpdateRotation(Rot);

	if (m_ColliderShape) m_ColliderShape->DrawShape(pRenderer, Pos);
}
