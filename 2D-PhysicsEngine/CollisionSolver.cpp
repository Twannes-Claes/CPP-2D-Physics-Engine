#include "CollisionSolver.h"
#include "Circle.h"
#include "Polygon.h"
#include "RigidBody.h"

#pragma warning(push)
#pragma warning(disable: 4201) // Disable warning C4201
#include <iostream>

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
		return CollisionSolver::CircleVSCircle(a, b, data);
	}

	if (aType == Shape::Type::Polygon && bType == Shape::Type::Polygon)
	{
		return CollisionSolver::PolyVSPoly(a, b, data);
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
	//https://turanszkij.files.wordpress.com/2020/04/sphere-sphere-2.png

	//Fill in contact data
	//Fill in rigidbodys
	data.a = a;
	data.b = b;

	////Projection method
	//Normal pointing from a to b
	data.normal = glm::normalize(glm::vec2{ b->Pos - a->Pos });
	
	//Get start and end location of collision between a && b
	data.start = b->Pos - (data.normal * bCircleShape->GetRadius());
	data.end = a->Pos + (data.normal * aCircleShape->GetRadius());
	
	//Get distance between start and end
	data.depth = glm::distance(data.end, data.start);
	
	ProjectionMethod(data);

	ImpulseMethod(data);

	return true;
}

bool CollisionSolver::PolyVSPoly(RigidBody* a, RigidBody* b, CollisionData& data)
{
	const Polygon* aPoly = dynamic_cast<Polygon*>(a->GetShape());
	const Polygon* bPoly = dynamic_cast<Polygon*>(b->GetShape());

	//If both seperations are less than zero they are colliding
	//If at least one is not negative return false no need to check twice

	CollisionDataPoly aInfo{};
	CollisionDataPoly bInfo{};

	FindLeastSeperation(aPoly, bPoly, aInfo);

	if (aInfo.seperationLength >= 0) return false;

	FindLeastSeperation(bPoly, aPoly, bInfo);

	if (bInfo.seperationLength >= 0) return false;

	data.a = a;
	data.b = b;
	
	if(aInfo.seperationLength > bInfo.seperationLength)
	{
		std::cout << "a to b\n";
		data.depth = -aInfo.seperationLength;
		data.normal = glm::normalize(glm::vec2{ -aInfo.seperationAxis.y, aInfo.seperationAxis.x });
	
		data.start = aInfo.projectedPoint;
		data.end = data.start + data.normal * data.depth;
	}
	else
	{
		std::cout << "b to a\n";
		data.depth = -bInfo.seperationLength;
		//data.normal = -glm::normalize(glm::vec2{ -bInfo.seperationAxis.y, bInfo.seperationAxis.x });
		data.normal = glm::normalize(glm::vec2{ bInfo.seperationAxis.y, -bInfo.seperationAxis.x });
	
		data.start = bInfo.projectedPoint - data.normal * data.depth;
		data.end = bInfo.projectedPoint;
	}
	
	ProjectionMethod(data);
	
	//ImpulseMethod(data);

	return true;
}

void CollisionSolver::ProjectionMethod(const CollisionData& data)
{
	if (data.a->IsStatic() && data.b->IsStatic()) return;

	//TODO ADD PERCENT CORRECTION
	//https://code.tutsplus.com/how-to-create-a-custom-2d-physics-engine-the-basics-and-impulse-resolution--gamedev-6331t
	//Projection method makes sure the shapes are set back to the correct position so they dont touch anymore.

	const float totalInvMass = data.a->InvMass + data.b->InvMass;

	const float da = data.depth / totalInvMass * data.a->InvMass;
	const float db = data.depth / totalInvMass * data.b->InvMass;
	
	data.a->Pos -= data.normal * da;
	data.b->Pos += data.normal * db;
}

void CollisionSolver::ImpulseMethod(const CollisionData& data)
{
	//https://code.tutsplus.com/how-to-create-a-custom-2d-physics-engine-the-basics-and-impulse-resolution--gamedev-6331t
	//Impulse method then response to that collision and moves the object corresponding

	//Relative Velocity and dot with normal
	//const float relVelDotNormal = glm::dot(data.a->Velocity - data.b->Velocity, data.normal);

	//Only check objects that are moving to eachother
	//This makes ojbects have a kind of relatio of sticking to eachother
	//if (relVelDotNormal > 0) return;

	//Now calculate relative velocity with angular velocity
	const glm::vec2 ra = data.end - data.a->Pos;
	const glm::vec2 rb = data.start - data.b->Pos;

	const glm::vec2 va = data.a->Velocity + glm::vec2{ -data.a->AngularVelocity * ra.y, data.a->AngularVelocity * ra.x };
	const glm::vec2 vb = data.b->Velocity + glm::vec2{ -data.b->AngularVelocity * rb.y, data.b->AngularVelocity * rb.x };

	//const float relVelDotNormal = glm::dot(va - vb, data.normal);
	const float relVelDotNormal = glm::dot(va - vb, data.normal);

	//Elasticity
	const float e = std::min(data.a->Restitution, data.b->Restitution);

	//Calculate the amount of impulse(scalar)
	const float j = -(1 - e) * relVelDotNormal / ((data.a->InvMass + data.b->InvMass) + ((Cross(ra, data.normal) * Cross(ra, data.normal)) / data.a->I) + ((Cross(rb, data.normal) * Cross(rb, data.normal)) / data.b->I));

	//Calculate full impulse force
	const glm::vec2 impulse = data.normal * j;

	//Add the impulses
	data.a->AddImpulse(impulse, ra);
	data.b->AddImpulse(-impulse, rb);
}

void CollisionSolver::FindLeastSeperation(const Polygon* a, const Polygon* b, CollisionDataPoly& data)
{
	//float seperation = FLT_MIN DOESNT WORK!!!!!!!!!!!!!!!! LOTS OF DEBUGGING!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	float seperation = -FLT_MAX;

	//Loop trough all vertices, and find the axis of the normal then loop trough the vertices off b and find the furthest point of that normal axis

	const auto& verticesA = a->GetVertices();
	const auto& verticesB = b->GetVertices();

	for(uint32_t i = 0; i < verticesA.size() - 1; ++i)
	{
		//Get the vertice
		const glm::vec2 vA = { verticesA[i].x,verticesA[i].y };

		// Get the edge of the vertice
		const glm::vec2 edge = a->GetEdge(i);

		// Calculate the normal by swapping x and inverting y
		const glm::vec2 n = glm::normalize(glm::vec2{ -edge.y, edge.x });

		float minSeperation = FLT_MAX;
		glm::vec2 bestProjectedPoint{};

		for (uint32_t j{}; j < verticesB.size() - 1; ++j)
		{
			//Get the vertice
			const glm::vec2 vB  = { verticesB[j].x, verticesB[j].y };

			//get the least seperation of the vertices
			const float projectionLength = glm::dot(vB - vA, n);
			if(projectionLength < minSeperation)
			{
				minSeperation = projectionLength;
				bestProjectedPoint = vB;
			}
		}

		//Check if the seperation found so far, is the least seperated if it has replace
		if(minSeperation > seperation)
		{
			seperation = minSeperation;
			data.seperationAxis = edge;
			data.projectedPoint = bestProjectedPoint;
		}
	}

	data.seperationLength = seperation;
}

float CollisionSolver::Cross(const glm::vec2& a, const glm::vec2& b)
{
	return (a.x * b.y) - (a.y * b.x);
}
