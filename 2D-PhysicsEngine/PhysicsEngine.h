#pragma once

#include <memory>
#include <vector>

#include "CollisionSolver.h"

class RigidBody;
class Font;
class CollisionSolver;

struct SDL_Window;
struct SDL_Renderer;
struct SDL_FPoint;

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

	std::vector<std::unique_ptr<RigidBody>> m_RigidBodies{};

	std::unique_ptr<Font> m_FontFPS{};
	std::unique_ptr<Font> m_FontFPSFixed{};
	std::unique_ptr<Font> m_FontAmountBodies{};

	//Physics Settings
	const float m_PixelsPerMeter = 25;
	const float m_MaxDeltaLag = 0.2f;
	const float m_Gravity = 9.81f;
	const float m_PhysicsTimeStep;

	CollisionSolver::CollisionData m_DataCollision{};

	int m_IndexCurr{};

	//FPS
	float m_DeltaTime = 0;
	float m_DeltaLag = 0;
	float m_FrameCount = 0;
	float m_FixedTotalTime = 0;

	float m_fpsTimer = 0;
	float m_DurationFixed = 0;
	float m_FixedLoopAmount = 0;

	std::vector<float> m_DurationsFixedData{};
	std::vector<uint32_t> m_AmountBodiesData{};
	uint32_t m_TotalDurationsData{};

	//MOusePos
	int m_MouseX{}, m_MouseY{};

	bool m_IsCtrlPressed{};

	void FixedUpdate();
	void NormalUpdate();

	void Draw() const;

	float Random(float min, float max) const;
	int Random(int min, int max) const;

	std::vector<SDL_FPoint> GenerateConvexPolygon(int numVertices, float radius) const;

	void PrintExperimentData() const;
	
};
