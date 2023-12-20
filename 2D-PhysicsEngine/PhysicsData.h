#pragma once

struct Vec2
{
	float x = 0;
	float y = 0;
};

struct Transform
{
	Vec2 pos{};
	float rotation{};
};

struct PhysicsMat
{
	float density{};
	float restitution{};
};

struct Mass
{
	float mass{};
	float invMass{};

	float inertia{};
	float invInertia{};
};

struct Body
{
	//Collider* pCollider

	Transform 

	//Velocity
	Vec2 v{};
	//Force
	Vec2 F{};



	//Maybe a gravity scale
	//float gravity scale = 0;
};




