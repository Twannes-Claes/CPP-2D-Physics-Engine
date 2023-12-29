#include "CollisionSolver.h"
#include "Circle.h"
#include "RigidBody.h"

#pragma warning(push)
#pragma warning(disable: 4201) // Disable warning C4201

#include "glm/gtx/norm.hpp"
#pragma warning(pop)

bool CollisionSolver::IsColliding(RigidBody* a, RigidBody* b, CollisionData& data)
{
	const Shape* aShape = a->GetShape();
	const Shape* bShape = b->GetShape();
	 
	const Shape::Type aType = aShape->GetType();
	const Shape::Type bType = bShape->GetType();

	if(aType == Shape::Type::Circle && bType == Shape::Type::Circle)
	{
		if (CollisionSolver::CircleVSCircle(a,b, data))
		{
			return true;
		}
	}

	return false;
}

bool CollisionSolver::CircleVSCircle(RigidBody* a, RigidBody* b, CollisionData& data)
{
	const Circle* aCircleShape = dynamic_cast<Circle*>(a->GetShape());
	const Circle* bCircleShape = dynamic_cast<Circle*>(b->GetShape());

	//Get the squared distance between the circles
	const float distance = glm::distance2(a->Pos, b->Pos);

	//Sum the radius
	const float circleRadiusSum = aCircleShape->GetRadius() + bCircleShape->GetRadius();

	//If distance squared is smaller than the radius squared circles are colliding
	const bool hasCollision = distance <= (circleRadiusSum * circleRadiusSum);

	//If no collision return early
	if(!hasCollision) return false;

	//Visualization of data
	//https://www.google.com/url?sa=i&url=https%3A%2F%2Fwickedengine.net%2F2020%2F04%2F26%2Fcapsule-collision-detection%2F&psig=AOvVaw2KBthMjEYGt4b6SxciEgYf&ust=1703900746464000&source=images&cd=vfe&opi=89978449&ved=0CBIQjRxqFwoTCPDV_IzDs4MDFQAAAAAdAAAAABBe

	//Fill in contact data
	//Fill in rigidbodys
	data.a = a;
	data.b = b;

	//Normal pointing from a to b
	data.normal = glm::normalize(glm::vec2{ b->Pos - a->Pos });

	//Get start and end location of collision between a && b
	data.start = b->Pos - (data.normal * bCircleShape->GetRadius());
	data.end = a->Pos + (data.normal * aCircleShape->GetRadius());

	//Get distance between start and end
	data.depth = glm::distance(data.end, data.start);

	const float da = data.depth / (data.a->InvMass + data.b->InvMass) * a->InvMass;
	const float db = data.depth / (data.a->InvMass + data.b->InvMass) * b->InvMass;

	a->Pos -= data.normal * da;
	b->Pos += data.normal * db;


	return true;
}
