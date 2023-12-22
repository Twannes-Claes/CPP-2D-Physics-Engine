#include "PhysicsEngine.h"

#include <chrono>
#include <iostream>
#include <string>
#include <vector>

#include "Box.h"
#include "Circle.h"
#include "SDL2/include/SDL.h"
#include "Font.h"
#include "Polygon.h"
#include "RigidBody.h"
#include "glm/geometric.hpp"

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

    //m_RigidBodys.push_back(std::make_unique<RigidBody>(100.f, 100.f, 1.f));
    //m_RigidBodys.push_back(std::make_unique<RigidBody>(135.f, 100.f, 10.f));

    m_RigidBodys.push_back(std::make_unique<RigidBody>(Box(100.f, 100.f),150.f, 100.f, 10.f));
    //m_RigidBodys.push_back(std::make_unique<RigidBody>(Circle(100.f), 250.f, 100.f, 10.f));
    //
    //std::vector<SDL_FPoint> points;
    //points.push_back(SDL_FPoint{ -50.f,-50.f });
    //points.push_back(SDL_FPoint{ 50.f,-50.f });
    //points.push_back(SDL_FPoint{ 0.f,50.f });
    //points.push_back(SDL_FPoint{ -50.f,-50.f });
    //
    //m_RigidBodys.push_back(std::make_unique<RigidBody>(Polygon(points), 400.f, 100.f, 1.f));

}

//Default assignment is needed in CPP for unique pointers
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
	       switch(event.type)
	       {
	           case SDL_QUIT:
		       {
	           	  quit = true;
	           	  break;
		       }
               case SDL_KEYDOWN:
               {
	               switch (const SDL_Keycode key = event.key.keysym.sym)
                   {
                        case SDLK_ESCAPE:
	                    {
                        	   quit = true;
                        	   break;
	                    }
                        //case SDLK_UP:
                        //{
                        //    yOffset -= m_PhysicsTimeStep * 100;
                        //    break;
                        //}
                        //case SDLK_DOWN:
                        //{
                        //    yOffset += m_PhysicsTimeStep * 100;
                        //    break;
                        //}
                        //case SDLK_LEFT:
                        //{
                        //    xOffset -= m_PhysicsTimeStep * 100;
                        //    break;
                        //}
                        //case SDLK_RIGHT:
                        //{
                        //    xOffset += m_PhysicsTimeStep * 100;
                        //    break;
                        //}
                        default:
                            break;
                   }
	           	   break;
               }
	           case SDL_MOUSEBUTTONDOWN:
		       {
                    //Setup for mouse movement to spawn objects
                   if (event.button.button == !SDL_BUTTON_LEFT) break;

                   int x{}, y{};
                   SDL_GetMouseState(&x, &y);

                   std::cout << "Clicked left mouse button: " << x << "/" << y << '\n';

                   break;
		       }
               default:
               {
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
    //Set a max amount of times the FixedUpdate can be called in a single frame
    if (m_DeltaLag > m_MaxDeltaLag) m_DeltaLag = m_MaxDeltaLag;

    //Fixed update loop that checks for missed timesteps
    while (m_DeltaLag >= m_PhysicsTimeStep)
    {
        //Update objects
        for (const auto& body : m_RigidBodys)
        {
            //Wind
            //body->AddForce(glm::vec2(m_PixelsPerMeter * 20.f, 0));
            //Gravity
            body->AddForce(glm::vec2(0.f, m_PixelsPerMeter * m_Gravity * body->mass));
            //body->GenerateDrag(0.00000001f);

            body->GenerateDrag(0.02f);
        }

        for (const auto& body : m_RigidBodys)
        {
            //Update
            body->Update(m_PhysicsTimeStep);
        }

        for (const auto& body : m_RigidBodys)
        {

            if (body->pos.y >= 550 || body->pos.y <= 50)
            {
                body->v.y *= -1;
            }
            if (body->pos.x <= 50 || body->pos.x >= 750)
            {
                body->v.x *= -1;
            }
        }

        m_DeltaLag -= m_PhysicsTimeStep;
    }
}

void PhysicsEngine::Draw() const
{
    //Clear render buffer
    SDL_SetRenderDrawColor(m_pRenderer, 0, 0, 0, 0);
    SDL_RenderClear(m_pRenderer);

    //Setup linear interpolation for drawing between 2 frames
    //This is for a smoothed transition
    //object.previous* a + object.curr * (1.f - a);

    //SDL_SetRenderDrawColor(m_pRenderer, 10, 10, 255, 200);
    //constexpr SDL_FRect rect{ 0, 300, 800, 300 };
    //SDL_RenderFillRectF(m_pRenderer, &rect);

    SDL_SetRenderDrawColor(m_pRenderer, 255, 255, 255, 255);

    for (const auto& body : m_RigidBodys)
    {
        body->Draw(m_pRenderer);
        //SDL_RenderDrawPointF(m_pRenderer, body->pos.x, body->pos.y);
    }

    m_FontFPS->Draw();

    //Push the buffer
    SDL_RenderPresent(m_pRenderer);
}