#include "PhysicsEngine.h"

#include <chrono>
#include <iostream>
#include <string>
#include <vector>

#include "SDL2/include/SDL.h"
#include "SDL2/include/SDL_ttf.h"

PhysicsEngine::PhysicsEngine(const int windowWidth, const int windowHeight, const float physicsTimeStep)
//Initialize the physicsTimeStep
:physicsTimeStep(physicsTimeStep)
{
    //Initialize SDL
    SDL_Init(SDL_INIT_VIDEO);

    //Create the window
    pWindow = SDL_CreateWindow("TwannesClaes-Engine", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, windowWidth, windowHeight, 0);
    pRenderer = SDL_CreateRenderer(pWindow, -1, SDL_RENDERER_ACCELERATED);

    //Disable VSYNC
    SDL_RenderSetVSync(pRenderer, 0);

    //Load font for FPSs
    if (TTF_Init() == -1) 
    {
        std::cerr << "SDL_ttf initialization failed: " << TTF_GetError() << std::endl;
    }

    pFont = TTF_OpenFont("ARCADECLASSIC.TTF", 24);

    if (!pFont) 
    {
        std::cerr << "TTF_OpenFont error: " << TTF_GetError() << std::endl;
    }

    pFontSurface = TTF_RenderText_Solid(pFont, "0", textColor);
    pFontTex = SDL_CreateTextureFromSurface(pRenderer, pFontSurface);
}

void PhysicsEngine::Run()
{
    //Variables needed for update loop, reference for SDL input and condition if loop has ended
    SDL_Event event;
    bool quit = false;

    //Get the start time
    auto startTime = std::chrono::high_resolution_clock::now();
    auto currTime = startTime;
    float delta = 0;

    float fpsTimer = 0;

    //If quit is true exit the game loop
    while (!quit)
    {
        //Get SDL events
        while (SDL_PollEvent(&event) != 0)
        {
            //Get if the window closes
            if (event.type == SDL_QUIT)
            {
                quit = true;
            }
            //Register if a key is down
            if(event.type == SDL_KEYDOWN)
            {
	            const SDL_Keycode key = event.key.keysym.sym;

                if (key == SDLK_ESCAPE) quit = true;

                switch(key)
                {
					case SDLK_UP:
					{
                        yOffset -= physicsTimeStep * 100;
						break;
					}
                    case SDLK_DOWN:
                    {
                        yOffset += physicsTimeStep * 100;
                        break;
                    }
                    case SDLK_LEFT:
                    {
                        xOffset -= physicsTimeStep * 100;
                        break;
                    }
                    case SDLK_RIGHT:
                    {
                        xOffset += physicsTimeStep * 100;
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
        currTime = std::chrono::high_resolution_clock::now();
        delta = std::chrono::duration<float>(currTime - startTime).count();

        startTime = currTime;
        elapsedSeconds += delta;

        fpsTimer += delta;

        // Calculate average FPS every second
        if (fpsTimer >= 1.f)
        {
            pFontSurface = TTF_RenderText_Solid(pFont, std::to_string(static_cast<int>(1/elapsedSeconds)).c_str(), textColor);
            pFontTex = SDL_CreateTextureFromSurface(pRenderer, pFontSurface);

            //Reset timer
            fpsTimer = 0;
        }
    }

    //Delete fonts
    SDL_FreeSurface(pFontSurface);
    SDL_DestroyTexture(pFontTex);
    TTF_CloseFont(pFont);

    //Delete the SDL window/renderer
    SDL_DestroyRenderer(pRenderer);
    SDL_DestroyWindow(pWindow);

    //QUit font and SDL
    TTF_Quit();
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

    //Draw Font
    const SDL_Rect textRect = { 10, 10, pFontSurface->w, pFontSurface->h };
    SDL_RenderCopy(pRenderer, pFontTex, nullptr, &textRect);

    // Present the renderer
    SDL_RenderPresent(pRenderer);
}