#include "PhysicsEngine.h"

#include <chrono>
#include <iostream>
#include <string>
#include <vector>

#include "SDL2/include/SDL.h"
#include "Font.h"

PhysicsEngine::PhysicsEngine(const int windowWidth, const int windowHeight, const float physicsTimeStep)
//Initialize the physicsTimeStep
:m_PhysicsTimeStep(physicsTimeStep)
{
    //Initialize SDL
    SDL_Init(SDL_INIT_VIDEO);

    //Create the window
    m_pWindow = SDL_CreateWindow("TwannesClaes-Engine", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, windowWidth, windowHeight, 0);
    m_pRenderer = SDL_CreateRenderer(m_pWindow, -1, SDL_RENDERER_ACCELERATED);

    //Disable VSYNC
    SDL_RenderSetVSync(m_pRenderer, 0);

    m_FontFPS = std::make_unique<Font>("ARCADECLASSIC.TTF", 24, SDL_Color{ 255, 255, 255, 255 }, 10, 10, m_pRenderer);
}

PhysicsEngine::~PhysicsEngine() = default;

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
                      yOffset -= m_PhysicsTimeStep * 100;
						break;
					}
                   case SDLK_DOWN:
                   {
                       yOffset += m_PhysicsTimeStep * 100;
                       break;
                   }
                   case SDLK_LEFT:
                   {
                       xOffset -= m_PhysicsTimeStep * 100;
                       break;
                   }
                   case SDLK_RIGHT:
                   {
                       xOffset += m_PhysicsTimeStep * 100;
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
        m_DeltaTime = delta;
        m_DeltaLag += delta;

        fpsTimer += delta;
        ++m_FrameCount;

        // Calculate average FPS every second
        if (fpsTimer >= 1.f)
        {
            m_FontFPS->SetText(std::to_string(static_cast<int>(m_FrameCount/ fpsTimer)).c_str());

            //Reset timer
            m_FrameCount = 0;
            fpsTimer = 0;
        }
    }

    //Delete the SDL window/renderer
    SDL_DestroyRenderer(m_pRenderer);
    SDL_DestroyWindow(m_pWindow);

    //Quit SDL
    SDL_Quit();
}

void PhysicsEngine::FixedUpdate()
{
    //Fixed update loop that checks for missed timesteps
    while (m_DeltaLag >= m_PhysicsTimeStep)
    {
        //Update objects
        //pObject.Update(physicsTimeStep);

        m_DeltaLag -= m_PhysicsTimeStep;
    }
}

void PhysicsEngine::Draw() const
{
    //Clear render buffer
    SDL_SetRenderDrawColor(m_pRenderer, 0, 0, 0, 0);
    SDL_RenderClear(m_pRenderer);

    //Test to draw triangles
    SDL_SetRenderDrawColor(m_pRenderer, 0, 0, 255, 255);
    std::vector<SDL_FPoint> points = { {100, 100}, {200, 100}, {150, 200}, {100, 100} };
    
    for (SDL_FPoint& point : points)
    {
        point.x += xOffset;
        point.y += yOffset;
    }

    SDL_RenderDrawLinesF(m_pRenderer, points.data(), static_cast<int>(points.size()));
    //

    m_FontFPS->Draw();

    //Push the buffer
    SDL_RenderPresent(m_pRenderer);
}