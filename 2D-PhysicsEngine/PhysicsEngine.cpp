#include "PhysicsEngine.h"

#include <chrono>
#include <iostream>
#include <vector>

#include "SDL2/include/SDL.h"

PhysicsEngine::PhysicsEngine(const int windowWidth, const int windowHeight, const float physicsTimeStep)
:physicsTimeStep(physicsTimeStep)
{
    //Initialize SDL
    SDL_Init(SDL_INIT_VIDEO);

    //Create the window
    pWindow = SDL_CreateWindow("TwannesClaes-Engine", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, windowWidth, windowHeight, 0);
    pRenderer = SDL_CreateRenderer(pWindow, -1, SDL_RENDERER_ACCELERATED);

    SDL_RenderSetVSync(pRenderer, 0);
}

void PhysicsEngine::Run()
{
    SDL_Event event;
    bool quit = false;

    auto startTime = std::chrono::high_resolution_clock::now();

    while (!quit)
    {
        //Get input
        while (SDL_PollEvent(&event) != 0)
        {
            //Get if the window closes
            if (event.type == SDL_QUIT)
            {
                quit = true;
            }
            if(event.type == SDL_KEYDOWN)
            {
	            const SDL_Keycode key = event.key.keysym.sym;

                if (key == SDLK_ESCAPE) quit = true;

                switch(key)
                {
					case SDLK_UP:
					{
                        yOffset -= elapsedSeconds * 100;
						break;
					}
                    case SDLK_DOWN:
                    {
                        yOffset += elapsedSeconds * 100;
                        break;
                    }
                    case SDLK_LEFT:
                    {
                        xOffset -= elapsedSeconds * 100;
                        break;
                    }
                    case SDLK_RIGHT:
                    {
                        xOffset += elapsedSeconds * 100;
                        break;
                    }
                    default:
                        break;
                }
            }
        }

        //Fixed update loop
        FixedUpdate();

        //Draw loop
        Draw();

        //Calculate elapsed seconds
        auto currTime = std::chrono::high_resolution_clock::now();
        const auto delta = std::chrono::duration<float>(currTime - startTime).count();

        startTime = currTime;
        elapsedSeconds += delta;

        std::cout << 1/elapsedSeconds << '\n';
    }

    SDL_DestroyRenderer(pRenderer);
    SDL_DestroyWindow(pWindow);
    SDL_Quit();
}

void PhysicsEngine::FixedUpdate()
{
    //Fixed update loop that checks for missed timesteps
    while (elapsedSeconds >= physicsTimeStep)
    {
        //Update objects
        //pObject.Update(physicsTimeStep);

        elapsedSeconds -= physicsTimeStep;
    }
}

void PhysicsEngine::Draw() const
{
    //Clear render buffer
    SDL_SetRenderDrawColor(pRenderer, 0, 0, 0, 0);
    SDL_RenderClear(pRenderer);

    //Test to draw triangles
    SDL_SetRenderDrawColor(pRenderer, 0, 0, 255, 255);
    std::vector<SDL_FPoint> points = { {100, 100}, {200, 100}, {150, 200}, {100, 100} };
    
    for (SDL_FPoint& point : points)
    {
        point.x += xOffset;
        point.y += yOffset;
    }

    SDL_RenderDrawLinesF(pRenderer, points.data(), static_cast<int>(points.size()));

    // Present the renderer
    SDL_RenderPresent(pRenderer);
}
