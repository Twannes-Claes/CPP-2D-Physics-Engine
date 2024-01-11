#include "RigidBody.h"

#pragma warning(push)
#pragma warning(disable: 4201) // Disable warning C4201
#include "Circle.h"
#include "glm/gtx/norm.hpp"
#pragma warning(pop)

const float RigidBody::m_PI2 = static_cast<float>(M_PI) * 2.f;

RigidBody::RigidBody(const Shape& colliderShape, const float x, const float y, float const mass, const float restitution, const float friction, const float rot) :
Pos(x, y),
Mass(mass),
Rot(rot),
Elasticity(restitution),
Friction(friction)
{
	//Get the inverse of the mass
	InvMass = 0;
	
	if (mass > 0) InvMass = 1 / mass;

	//Implemented prototype pattern for easy transfer of shapes
	//No need to std::make_unique<ShapeType>(all the arguments)
	//Now every shape class has a function clone that returns an unique pointer with its own arguments
	//Learned this in programming 4 https://gameprogrammingpatterns.com/prototype.html
	m_ColliderShape = colliderShape.Clone();

	m_ColliderShape->UpdatePosRot(0, Pos);

	I = m_ColliderShape->GetMomentOfInteria(mass);

	InvI = 0;

	if (I > 0.f) InvI = 1 / I;

	m_ColliderShape->UpdatePosRot(Rot, Pos);
	m_ColliderShape->UpdateVertices();
}

RigidBody::RigidBody(const Shape& colliderShape, const int x, const int y, const float mass, const float restitution, const float friction, const float rot)
:RigidBody(colliderShape, static_cast<float>(x), static_cast<float>(y), mass, restitution, friction, rot){}

RigidBody::~RigidBody() = default;

void RigidBody::AddImpulse(const glm::vec2& impulse)
{
	if (IsStatic()) return; Velocity += impulse * InvMass;
}

void RigidBody::AddImpulse(const glm::vec2& impulse, const glm::vec2& dir)
{
	if(IsStatic()) return;

	Velocity += impulse * InvMass;
	AngularVelocity += ((dir.x * impulse.y) - (dir.y * impulse.x)) * InvI;
}

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
	//Static objects shouldnt update
	if(IsStatic()) return;

	//Euler integration of acceleration
	m_Acceleration = m_AccumulatedForce * InvMass;

	Velocity += m_Acceleration * deltaTime;

	Pos += Velocity * deltaTime;

	//Clear linear forces
	m_AccumulatedForce = glm::vec2{};

	//Euler integration of rotation acceleration
	m_AngularAcceleration = m_AccumulatedTorque * InvI;

	AngularVelocity += m_AngularAcceleration * deltaTime;

	Rot += AngularVelocity * deltaTime;

	Rot = std::fmodf(Rot, m_PI2);

	//Clear torque force
	m_AccumulatedTorque = 0;

	//Update rot of the shape to then recalculate world space
	m_ColliderShape->UpdatePosRot(Rot, Pos);
	m_ColliderShape->UpdateVertices();
}

void RigidBody::Draw(SDL_Renderer* pRenderer) const
{
	//if(colliding)
	//{
	//
	//	SDL_SetRenderDrawColor(pRenderer, 255, 0, 0, 255);
	//}
	//else
	//{
	//	SDL_SetRenderDrawColor(pRenderer, 255, 255, 255, 255);
	//}

	SDL_SetRenderDrawColor(pRenderer, 255, 255, 255, 255);

	if (m_ColliderShape) m_ColliderShape->DrawShape(pRenderer);
}
