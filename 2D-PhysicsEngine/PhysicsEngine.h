#pragma once

#include <memory>

class Font;

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

	//Other variables
	float xOffset = 0, yOffset = 0;

	std::unique_ptr<Font> m_FontFPS;

	//Physics Settings
	const float m_MaxDeltaLag = 0.2f;
	const float m_Gravity = -9.81f;
	const float m_PhysicsTimeStep;

	//FPS
	float m_DeltaTime = 0;
	float m_DeltaLag = 0;
	float m_FrameCount = 0;

	void FixedUpdate();

	void Draw() const;
};
