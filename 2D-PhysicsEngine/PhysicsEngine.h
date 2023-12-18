#pragma once
#include <SDL_pixels.h>
#include <SDL_ttf.h>

struct SDL_Window;
struct SDL_Renderer;
struct SDL_Surface;
struct SDL_Texture;

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

	//FPS
	float elapsedSeconds = 0;

	const SDL_Color textColor = { 255, 255, 255, 255 };

	SDL_Surface* pFontSurface = nullptr;
	SDL_Texture* pFontTex = nullptr;
	TTF_Font* pFont = nullptr;

	void FixedUpdate();

	void Draw() const;
};

