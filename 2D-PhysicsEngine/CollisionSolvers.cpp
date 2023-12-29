#include "CollisionSolvers.h"
#include "Circle.h"

#pragma warning(push)
#pragma warning(disable: 4201) // Disable warning C4201
#include "RigidBody.h"
#include "glm/gtx/norm.hpp"
#pragma warning(pop)

bool CollisionSolver::IsColliding(RigidBody* a, RigidBody* b, CollisionData& data)
{
	Shape* aShape = a->GetShape();
	Shape* bShape = b->GetShape();
	 
	const Shape::Type aType = aShape->GetType();
	const Shape::Type bType = bShape->GetType();

	if(aType == Shape::Type::Circle && bType == Shape::Type::Circle)
	{
		if (CircleVSCircle(a,b, data))
		{
			return true;
		}
	}

	return false;
}

bool CollisionSolver::CircleVSCircle(RigidBody* a, RigidBody* b, CollisionData&)
{
	const Circle* aCircleShape = dynamic_cast<Circle*>(a->GetShape());
	const Circle* bCircleShape = dynamic_cast<Circle*>(b->GetShape());

	//Get the squared distance between the circles
	const float distance = glm::distance2(a->Pos, b->Pos);

	//Sum the radius
	const float circleRadiusSum = aCircleShape->GetRadius() + bCircleShape->GetRadius();

	//If distance squared is smaller than the radius squared circles are colliding
	return distance <= (circleRadiusSum * circleRadiusSum);
}
