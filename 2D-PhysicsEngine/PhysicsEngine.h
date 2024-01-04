#pragma once

#include <memory>
#include <vector>

#include "CollisionSolver.h"

class RigidBody;
class Font;
class CollisionSolver;

struct SDL_Window;
struct SDL_Renderer;

class PhysicsEngine
{
public:

	//BIG SIX
	PhysicsEngine(int windowWidth, int windowHeight, float physicsTimeStep);
	~PhysicsEngine();

	PhysicsEngine(const PhysicsEngine& other) = delete;
	PhysicsEngine(PhysicsEngine&& other) = delete;
	PhysicsEngine& operator=(const PhysicsEngine& other) = delete;
	PhysicsEngine& operator=(PhysicsEngine&& other) = delete;

	void Run();

private:

	SDL_Window* m_pWindow = nullptr;
	SDL_Renderer* m_pRenderer = nullptr;

	std::vector<std::unique_ptr<RigidBody>> m_RigidBodys;

	std::unique_ptr<Font> m_FontFPS;
	std::unique_ptr<Font> m_FontFPSFixed;
	std::unique_ptr<Font> m_FontAmountBodies;

	//Physics Settings
	const float m_PixelsPerMeter = 25;
	const float m_MaxDeltaLag = 0.2f;
	const float m_Gravity = 9.81f;
	const float m_PhysicsTimeStep;

	CollisionSolver::CollisionData m_DataCollision;

	//FPS
	float m_DeltaTime = 0;
	float m_DeltaLag = 0;
	float m_FrameCount = 0;

	float m_fpsTimer = 0;
	float m_DurationFixed = 0;

	void FixedUpdate();

	void Draw() const;
};
