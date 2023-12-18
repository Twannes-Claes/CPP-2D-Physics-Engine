#pragma once
struct SDL_Window;
struct SDL_Renderer;

class PhysicsEngine
{
public:

	//BIG SIX
	PhysicsEngine(int windowWidth, int windowHeight, float physicsTimeStep);
	~PhysicsEngine() = default;

	PhysicsEngine(const PhysicsEngine& other) = delete;
	PhysicsEngine(PhysicsEngine&& other) = delete;
	PhysicsEngine& operator=(const PhysicsEngine& other) = delete;
	PhysicsEngine& operator=(PhysicsEngine&& other) = delete;

	void Run();

private:

	SDL_Window* pWindow = nullptr;
	SDL_Renderer* pRenderer = nullptr;

	float xOffset = 0, yOffset = 0;

	const float physicsTimeStep;
	float elapsedSeconds = 0;

	void FixedUpdate();

	void Draw() const;
};

