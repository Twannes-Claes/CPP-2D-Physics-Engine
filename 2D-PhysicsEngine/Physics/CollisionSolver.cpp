#include "CollisionSolver.h"

#include "Circle.h"
#include "Polygon.h"
#include "RigidBody.h"

#pragma warning(push)
#pragma warning(disable: 4201) // Disable warning C4201
#include "../glm/gtx/norm.hpp"
#pragma warning(pop)

#define SHAPE_TYPE Shape::Type

//#define DEBUG_DETECTION

bool CollisionSolver::IsColliding(RigidBody* a, RigidBody* b, CollisionData& data)
{
	const SHAPE_TYPE cacheBType = b->GetShape()->GetType();

	switch (a->GetShape()->GetType())
	{
		case SHAPE_TYPE::Circle:
		{
			switch (cacheBType)
			{
				case SHAPE_TYPE::Circle:
				{
					return CircleVSCircle(a, b, data, false);
				}
				case SHAPE_TYPE::Polygon:
				case SHAPE_TYPE::Box:
				{
					return PolyVsCircle(b, a, data);
				}
			}
			break;
		}

		case SHAPE_TYPE::Polygon:
		case SHAPE_TYPE::Box:
		{
			switch (cacheBType)
			{
				case SHAPE_TYPE::Circle:
				{
					return PolyVsCircle(a, b, data);
				}
				case SHAPE_TYPE::Polygon:
				case SHAPE_TYPE::Box:
				{
					return PolyVSPoly(a, b, data);
				}
			}
		}
		break;
	}

	return false;
}

bool CollisionSolver::CircleVSCircle(RigidBody* a, RigidBody* b, CollisionData& data, const bool detectOnly)
{
	const Shape* aCircleShape = a->GetShape();
	const Shape* bCircleShape = b->GetShape();

	//Get the squared distance between the circles
	const float distance = distance2(a->Pos, b->Pos);

	//Sum the radius
	const float circleRadiusSum = aCircleShape->GetBoundingRadius() + bCircleShape->GetBoundingRadius();

	//If distance squared is smaller than the radius squared circles are colliding
	const bool hasCollision = distance <= (circleRadiusSum * circleRadiusSum);

	//If no collision return early
	if(!hasCollision) return false;

	if (detectOnly) return true;

	//Visualization of data
	//https://turanszkij.files.wordpress.com/2020/04/sphere-sphere-2.png

	//Fill in contact data
	//Fill in rigidbodys
	data.a = a;
	data.b = b;

	////Projection method
	//Normal pointing from a to b
	data.normal = normalize(glm::vec2{ b->Pos - a->Pos });
	
	//Get start and end location of collision between a && b
	data.start = b->Pos - (data.normal * bCircleShape->GetBoundingRadius());
	data.end = a->Pos + (data.normal * aCircleShape->GetBoundingRadius());
	
	//Get distance between start and end
	data.depth = glm::distance(data.end, data.start);

	#ifndef DEBUG_DETECTION
	PositionalCorrection(data);

	AddImpulses(data);
	#endif

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

	if (aInfo.seperationLength >= 0)
	{
		return false;
	}

	FindLeastSeperation(bPoly, aPoly, bInfo);

	if (bInfo.seperationLength >= 0) return false;

	//Other wise collision has been found
	//FIll in the data
	data.a = a;
	data.b = b;
	
	if(aInfo.seperationLength > bInfo.seperationLength)
	{
		data.depth = -aInfo.seperationLength;
		data.normal = normalize(glm::vec2{ -aInfo.seperationAxis.y, aInfo.seperationAxis.x });
	
		data.start = aInfo.projectedPoint;
		data.end = data.start + data.normal * data.depth;
	}
	else
	{
		data.depth = -bInfo.seperationLength;
		//data.normal = -glm::normalize(glm::vec2{ -bInfo.seperationAxis.y, bInfo.seperationAxis.x });
		data.normal = normalize(glm::vec2{ bInfo.seperationAxis.y, -bInfo.seperationAxis.x });
	
		data.start = bInfo.projectedPoint - data.normal * data.depth;
		data.end = bInfo.projectedPoint;
	}

	#ifndef DEBUG_DETECTION
	PositionalCorrection(data);
	
	AddImpulses(data);
	#endif

	return true;
}

void CollisionSolver::FindLeastSeperation(const Polygon* a, const Polygon* b, CollisionDataPoly& data)
{
	//float seperation = FLT_MIN DOESNT WORK!!!!!!!!!!!!!!!! LOTS OF DEBUGGING!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	float seperation = -FLT_MAX;

	//Loop trough all vertices, and find the axis of the normal then loop trough the vertices off b and find the furthest point of that normal axis

	const auto& verticesA = a->GetVertices();
	const auto& verticesB = b->GetVertices();

	for (uint32_t i = 0; i < verticesA.size(); ++i)
	{
		//Get the vertice
		const glm::vec2 vA = { verticesA[i].x,verticesA[i].y };

		// Get the edge of the vertice
		const glm::vec2 edge = a->GetEdge(i);

		// Calculate the normal by swapping x and inverting y
		const glm::vec2 n = normalize(glm::vec2{ -edge.y, edge.x });

		float minSeperation = FLT_MAX;
		glm::vec2 bestProjectedPoint{};

		for (uint32_t j{}; j < verticesB.size(); ++j)
		{
			//Get the vertice
			const glm::vec2 vB = { verticesB[j].x, verticesB[j].y };
			//get the least seperation of the vertices
			const float projectionLength = dot(vB - vA, n);
			if (projectionLength < minSeperation)
			{
				minSeperation = projectionLength;
				bestProjectedPoint = vB;
			}
		}

		//Check if the seperation found so far, is the least seperated if it has replace
		if (minSeperation > seperation)
		{
			seperation = minSeperation;
			data.seperationAxis = edge;
			data.projectedPoint = bestProjectedPoint;
		}
	}

	data.seperationLength = seperation;
}

bool CollisionSolver::PolyVsCircle(RigidBody* polyBody, RigidBody* circleBody, CollisionData& data)
{
	const Polygon* poly = dynamic_cast<Polygon*>(polyBody->GetShape());
	const Circle* circle = dynamic_cast<Circle*>(circleBody->GetShape());

	const auto& verticesPoly = poly->GetVertices();

	bool isCircleOutsidePoly = false;

	glm::vec2 minCurrV{};
	glm::vec2 minNextV{};

	float distanceToEdge = -FLT_MAX;

	for (int i = 0; i < static_cast<int>(verticesPoly.size()); ++i)
	{
		//Get the next vertice index
		const int nextI = (i + 1) % static_cast<int>(verticesPoly.size());

		const glm::vec2 currV = glm::vec2{ verticesPoly[i].x, verticesPoly[i].y };
		const glm::vec2 nextV = glm::vec2{ verticesPoly[nextI].x, verticesPoly[nextI].y };

		//Calculate the edge and get the normal
		const glm::vec2 edge = poly->GetEdge(i);
		//DONT FORGEOT TO FKING NORMALIZE YOUR FUCKING NORMALS TWANNES YOU RETARD 2 HOURS OF DEBUGGING JEEEZUS
		const glm::vec2 n = normalize(glm::vec2{ -edge.y, edge.x });

		//Get the vector between the vertice and the circle
		const glm::vec2 verticeToCircle = circleBody->Pos - currV;

		//Project the vector on the normal of the edge to find where it is
		const float dotProjection = dot(verticeToCircle, n);

		//If projection is larger than zero then it means that the circle is outside the polygon
		if(dotProjection > 0)
		{
			distanceToEdge = dotProjection;
			
			minCurrV = currV;
			minNextV = nextV;

			isCircleOutsidePoly = true;
			break;
		}

		//In the other case the circle position is inside the polgyon
		if(dotProjection > distanceToEdge)
		{
			distanceToEdge = dotProjection;

			minCurrV = currV;
			minNextV = nextV;

			isCircleOutsidePoly = false;
		}
	}

	const float radius = circle->GetBoundingRadius();

	if(isCircleOutsidePoly)
	{
		//Check the three regions
		//1. Left of current vertex
		//again vector from circle to vertex but this time from the found min vertex
		const glm::vec2 vecCircleToVertice = circleBody->Pos - minCurrV;
		const glm::vec2 edge = minNextV - minCurrV;

		//If projection is less than zero it means it is left of the curr vertex
		if (dot(vecCircleToVertice, edge) < 0)
		{
			//Now check if circle collides with currVertex, with distnace check
			const float distance = length(vecCircleToVertice);
			if (distance > radius) return false;

			//Fill in data
			data.a = polyBody;
			data.b = circleBody;

			data.depth = radius - distance;
			data.normal = normalize(vecCircleToVertice);

			data.start = circleBody->Pos + -data.normal * radius;
			data.end = data.start + data.normal * data.depth;
		}
		else
		{

			//2. Right of the next vertex
			//Get vector from circle to the next vertice
			const glm::vec2 vecCircleToVertice2 = circleBody->Pos - minNextV;
			//Get the reverse of the edge starting form the other vertice
			const glm::vec2 edge2 = minCurrV - minNextV;

			//If projection is less than zero it means it is right of the next vertex
			if (dot(vecCircleToVertice2, edge2) < 0)
			{
				//Now check if circle collides with nextVertex, with distnace check
				const float distance = length(vecCircleToVertice2);

				if (distance > radius) return false;

				//Fill in data
				data.a = polyBody;
				data.b = circleBody;

				data.depth = radius - distance;
				data.normal = normalize(vecCircleToVertice2);

				data.start = circleBody->Pos + -data.normal * radius;
				data.end = data.start + data.normal * data.depth;
			}
			else
			{
				//3. Along the edge of curr vertex and next vertex
				//Else we are along the edge

				//Reuse distance to edge to find if the circle is close enough in range
				if (distanceToEdge > radius)
				{
					return false;
				}

				//Fill in data
				data.a = polyBody;
				data.b = circleBody;

				data.depth = radius - distanceToEdge;
				//Get the normal of the edge
				const glm::vec2 edgeNormalized = normalize(glm::vec2{ minNextV - minCurrV });
				data.normal = glm::vec2{ -edgeNormalized.y, edgeNormalized.x };

				if (poly->GetType() == Shape::Type::Polygon)
				{
					data.normal *= -1.f;
				}

				data.start = circleBody->Pos - data.normal * radius;
				data.end = data.start + data.normal * data.depth;
			}
		}
	}
	else
	{
		//Else circle is inside of polygo
		//Fill in data
		data.a = polyBody;
		data.b = circleBody;

		data.depth = radius - distanceToEdge;
		//Get the normal of the edge
		const glm::vec2 edgeNormalized = normalize(glm::vec2{ minNextV - minCurrV });
		data.normal = glm::vec2{ -edgeNormalized.y, edgeNormalized.x };

		data.start = circleBody->Pos - data.normal * radius;

		data.end = data.start + data.normal * data.depth;

		if(poly->GetType() == Shape::Type::Polygon)
		{
			data.start = circleBody->Pos + data.normal * radius;
			data.normal *= -1;
			data.end += (circleBody->Pos - data.end) * 2.f;
		}
	}

	#ifndef DEBUG_DETECTION
	PositionalCorrection(data);
	
	AddImpulses(data);
	#endif

	return true;
}



void CollisionSolver::PositionalCorrection(const CollisionData& data)
{
	if (data.a->IsStatic() && data.b->IsStatic()) return;

	//https://code.tutsplus.com/how-to-create-a-custom-2d-physics-engine-the-basics-and-impulse-resolution--gamedev-6331t
	//Projection method makes sure the shapes are set back to the correct position so they dont touch anymore.

	constexpr float percent = 0.7f;
	constexpr float minPenetration = 0.01f;

	const float totalInvMass = data.a->InvMass + data.b->InvMass;

	//Get the length of how much the objects need to move
	//the std::max is to let objects penetrate for a small amount, this lowers jittering objects
	const glm::vec2 correctionLength = std::max( data.depth - minPenetration, 0.f) / totalInvMass * data.normal * percent;

	data.a->Pos -= data.a->InvMass * correctionLength;
	data.b->Pos += data.b->InvMass * correctionLength;

	//Because the position has been updated, we need to update vertices of the bodies to have a more smooth collision
	data.a->GetShape()->UpdatePosRot(data.a->Rot, data.a->Pos);
	data.a->GetShape()->UpdateVertices();
	data.b->GetShape()->UpdatePosRot(data.b->Rot, data.b->Pos);
	data.b->GetShape()->UpdateVertices();
}

void CollisionSolver::AddImpulses(const CollisionData& data)
{
	//https://code.tutsplus.com/how-to-create-a-custom-2d-physics-engine-the-basics-and-impulse-resolution--gamedev-6331t
	//Impulse method then response to that collision and moves the object corresponding

	//Only check objects that are moving to eachother
	//This makes ojbects have a kind of relatio of sticking to eachother
	//if (relVelDotNormal > 0) return;

	//get mininum Elasticity and friction
	const float e = std::min(data.a->Elasticity, data.b->Elasticity);
	const float f = std::min(data.a->Friction, data.b->Friction);

	//Get the contact directions between body a and b
	const glm::vec2 contactDirA = data.end - data.a->Pos;
	const glm::vec2 contactDirB = data.start - data.b->Pos;

	//Now calculate relative velocity with angular velocity
	const glm::vec2 relVelA = data.a->Velocity + glm::vec2{ -data.a->AngularVelocity * contactDirA.y, data.a->AngularVelocity * contactDirA.x };
	const glm::vec2 relVelB = data.b->Velocity + glm::vec2{ -data.b->AngularVelocity * contactDirB.y, data.b->AngularVelocity * contactDirB.x };

	//const float relVelDotNormal = glm::dot(va - vb, data.normal);
	const glm::vec2 relVel = relVelA - relVelB;

	const float relVelDotNormal = dot(relVel, data.normal);

	//Formula https://gamedev.stackexchange.com/questions/157537/impulse-resolution-for-purely-rotational-collisions-relative-linear-velocity
	//Calculate the amount of impulse(scalar)
	const float impulseLinearForce = -(1 + e) * relVelDotNormal / ((data.a->InvMass + data.b->InvMass) + ((Cross(contactDirA, data.normal) * Cross(contactDirA, data.normal)) * data.a->InvI) + ((Cross(contactDirB, data.normal) * Cross(contactDirB, data.normal)) * data.b->InvI));

	//Now rotational impulse along tangent
	const glm::vec2 tangent = glm::vec2{ -data.normal.y, data.normal.x };
	const float relVelDotTangent = dot(relVel, tangent);

	const float impulseTangentForce = f * -(1 + e) * relVelDotTangent / ((data.a->InvMass + data.b->InvMass) + ((Cross(contactDirA, tangent) * Cross(contactDirA, tangent)) * data.a->InvI) + ((Cross(contactDirB, tangent) * Cross(contactDirB, tangent)) * data.b->InvI));

	//Calculate full impulse with linear and rotational
	const glm::vec2 impulse = (data.normal * impulseLinearForce) + (tangent * impulseTangentForce);

	//Add the impulses
	data.a->AddImpulse(impulse, contactDirA);
	data.b->AddImpulse(-impulse, contactDirB);
}