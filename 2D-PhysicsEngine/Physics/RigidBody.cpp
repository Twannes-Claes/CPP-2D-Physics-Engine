#include "RigidBody.h"

#pragma warning(push)
#pragma warning(disable: 4201) // Disable warning C4201
#include "Circle.h"
#include "../glm/gtx/norm.hpp"
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
	if (IsStatic()) return;

	Velocity += impulse * InvMass;
}

void RigidBody::AddImpulse(const glm::vec2& impulse, const glm::vec2& dir)
{
	if(IsStatic()) return;

	Velocity += impulse * InvMass;
	AngularVelocity += ((dir.x * impulse.y) - (dir.y * impulse.x)) * InvI;
}

void RigidBody::Update(const float deltaTime)
{
	//Static objects shouldnt update
	if(IsStatic()) return;

	EulerIntegration(deltaTime);
	//VerletIntegration(deltaTime);
	//RK4Integration(deltaTime);

	//Rotation should always be in bounds of 360 degrees
	Rot = std::fmodf(Rot, m_PI2);

	//Clear linear and angular forces
	m_AccumulatedForce = glm::vec2{};
	m_AccumulatedTorque = 0;

	//Update rot of the shape to then recalculate world space
	m_ColliderShape->UpdatePosRot(Rot, Pos);
	m_ColliderShape->UpdateVertices();
}

void RigidBody::Draw(SDL_Renderer* pRenderer) const
{
	if (m_ColliderShape) m_ColliderShape->DrawShape(pRenderer);
}

void RigidBody::EulerIntegration(const float deltaTime)
{
	//Calculate the linear and angular acceleration
	m_Acceleration = m_AccumulatedForce * InvMass;
	m_AngularAcceleration = m_AccumulatedTorque * InvI;

	//Euler integration
	Velocity += m_Acceleration * deltaTime;

	Pos += Velocity * deltaTime;

	AngularVelocity += m_AngularAcceleration * deltaTime;

	Rot += AngularVelocity * deltaTime;
}

void RigidBody::VerletIntegration(const float deltaTime)
{
	//https://en.wikipedia.org/wiki/Verlet_integration
	//Verlet integration
	const glm::vec2 oldAcc = m_Acceleration;
	m_Acceleration = m_AccumulatedForce * InvMass;

	Pos = Pos + Velocity * deltaTime + m_Acceleration * (deltaTime * deltaTime / 2);
	Velocity += (m_Acceleration + oldAcc) * (deltaTime / 2);

	const float oldAccAngular = m_AngularAcceleration;
	m_AngularAcceleration = m_AccumulatedTorque * InvI;

	Rot = Rot + AngularVelocity * deltaTime + m_AngularAcceleration * (deltaTime * deltaTime / 2);
	AngularVelocity += (m_AngularAcceleration + oldAccAngular) * (deltaTime / 2);
}

void RigidBody::RK4Integration(const float deltaTime)
{
	m_Acceleration = m_AccumulatedForce * InvMass;
	m_AngularAcceleration = m_AccumulatedTorque * InvI;

	const glm::vec2 k1xy = Velocity * deltaTime;
	const glm::vec2 k1v = m_Acceleration * deltaTime;

	const glm::vec2 k2xy = (Velocity + 0.5f * k1v) * deltaTime;
	const glm::vec2 k2v = (m_Acceleration + 0.5f * k1v) * deltaTime;

	const glm::vec2 k3xy = (Velocity + 0.5f * k2v) * deltaTime;
	const glm::vec2 k3v = (m_Acceleration + 0.5f * k2v) * deltaTime;

	const glm::vec2 k4xy = (Velocity + k3v) * deltaTime;
	const glm::vec2 k4v = (m_Acceleration + k3v) * deltaTime;

	Pos += (k1xy + 2.f * k2xy + 2.f * k3xy + k4xy) / 6.f;
	Velocity += (k1v + 2.f * k2v + 2.f * k3v + k4v) / 6.f;

	const float k1xyRot = AngularVelocity * deltaTime;
	const float k1vRot = m_AngularAcceleration * deltaTime;
		 
	const float k2xyRot = (AngularVelocity + 0.5f * k1vRot) * deltaTime;
	const float k2vRot = (m_AngularAcceleration + 0.5f * k1vRot) * deltaTime;
		 
	const float k3xyRot = (AngularVelocity + 0.5f * k2vRot) * deltaTime;
	const float k3vRot = (m_AngularAcceleration + 0.5f * k2vRot) * deltaTime;
		 
	const float k4xyRot = (AngularVelocity + k3vRot) * deltaTime;
	const float k4vRot = (m_AngularAcceleration + k3vRot) * deltaTime;

	Rot += (k1xyRot + 2.f * k2xyRot + 2.f * k3xyRot + k4xyRot) / 6.f;
	AngularVelocity += (k1vRot + 2.f * k2vRot + 2.f * k3vRot + k4vRot) / 6.f;

}
