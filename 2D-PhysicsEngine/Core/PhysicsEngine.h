#pragma once

#include <memory>
#include <vector>

class PhysicsWorld;
class Font;

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

	std::unique_ptr<PhysicsWorld> m_PhysicsWorld{};

	std::unique_ptr<Font> m_FontFPS{};
	std::unique_ptr<Font> m_FontFPSFixed{};
	std::unique_ptr<Font> m_FontAmountBodies{};

	//Physics Settings
	const float m_PixelsPerMeter = 25;
	const float m_MaxDeltaLag = 0.2f;
	const float m_PhysicsTimeStep;

	int m_IndexCurr{};

	//FPS
	float m_DeltaTime = 0;
	float m_DeltaLag = 0;
	float m_FrameCount = 0;
	float m_FixedTotalTime = 0;

	float m_fpsTimer = 0;
	float m_DurationFixed = 0;
	float m_FixedLoopAmount = 0;

	//MousePos
	int m_MouseX{}, m_MouseY{};

	void FixedUpdate();

	void Draw() const;

	//Random polygon creation
	float Random(float min, float max) const;
	int Random(int min, int max) const;
	std::vector<SDL_FPoint> GenerateConvexPolygon(int numVertices, float radius) const;

	void SetupRendering(const int w, const int h);
	void SetupPhysics(const int w, const int h);

	void UpdateText();
	
};
