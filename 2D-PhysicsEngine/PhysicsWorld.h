#pragma once
#include <vector>

#include "Physics/CollisionSolver.h"
#include "Physics/RigidBody.h"

struct SDL_Renderer;

class PhysicsWorld
{
public:
	explicit PhysicsWorld(const glm::vec2& gravity = glm::vec2{ 0.f, 9.81f }, const int pixelsPerMeter = 25) :Gravity(gravity), m_PixelsPerMeter(pixelsPerMeter) {}
	~PhysicsWorld();

	PhysicsWorld(const PhysicsWorld& other) = delete;
	PhysicsWorld(PhysicsWorld&& other) = delete;
	PhysicsWorld& operator=(const PhysicsWorld& other) = delete;
	PhysicsWorld& operator=(PhysicsWorld&& other) = delete;

	void Update(const float timeStep);
	void Draw(SDL_Renderer* pRenderer) const;

	void AddBody(const Shape& colliderShape, float x, float y, float mass, float restitution = 0.5f, float friction = 0.5f, float rot = 0.f);
	void AddBody(const Shape& colliderShape, int x, int y, float mass, float restitution = 0.5f, float friction = 0.5f, float rot = 0.f);

	uint32_t AmountBodies() const { return static_cast<uint32_t>(m_RigidBodies.size()); }
	RigidBody* GetBodyAtIndex(const uint32_t index) const { return m_RigidBodies[index].get(); }
	const std::vector<std::unique_ptr<RigidBody>>& GetBodies() const { return m_RigidBodies; }

	//Can be used as a property, hence public
	glm::vec2 Gravity{};

private:

	std::vector<std::unique_ptr<RigidBody>> m_RigidBodies{};

	CollisionSolver::CollisionData m_DataCollision{};

	const int m_PixelsPerMeter{};
};


