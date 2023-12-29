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
#include "CollisionSolver.h"

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

    //m_RigidBodys.push_back(std::make_unique<RigidBody>(Box(100.f, 50.f),150.f, 100.f, 1.f));
    m_RigidBodys.push_back(std::make_unique<RigidBody>(Circle(150.f), 400.f, 300.f, -1.f));
    //m_RigidBodys.push_back(std::make_unique<RigidBody>(Circle(50.f), 150.f, 100.f, 2.f));
    //m_RigidBodys.push_back(std::make_unique<RigidBody>(Circle(50.f), 250.f, 100.f, 3.f));
    //m_RigidBodys.push_back(std::make_unique<RigidBody>(Circle(50.f), 150.f, 100.f, 4.f));
    //m_RigidBodys.push_back(std::make_unique<RigidBody>(Circle(50.f), 50.f, 100.f, 5.f));
    //m_RigidBodys.push_back(std::make_unique<RigidBody>(Circle(50.f), 250.f, 100.f, 6.f));
    //m_RigidBodys.push_back(std::make_unique<RigidBody>(Circle(50.f), 350.f, 100.f, 7.f));
    //m_RigidBodys.push_back(std::make_unique<RigidBody>(Circle(50.f), 150.f, 100.f, 8.f));
    //
    //std::vector<SDL_FPoint> points;
    //points.push_back(SDL_FPoint{ -50.f,-50.f });
    //points.push_back(SDL_FPoint{ 50.f,-50.f });
    //points.push_back(SDL_FPoint{ 0.f,50.f });
    //points.push_back(SDL_FPoint{ -50.f,-50.f });
    ////
    //m_RigidBodys.push_back(std::make_unique<RigidBody>(Polygon(points), 400.f, 100.f, 100.f));

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

                   m_RigidBodys.push_back(std::make_unique<RigidBody>(Circle(10.f), static_cast<float>(x), static_cast<float>(y), 1.f));

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
        for (const auto& body : m_RigidBodys)
        {
            //Add wind
            //body->AddForce(glm::vec2{ 20.f, 0.f });

            //Gravity
            body->AddForce(glm::vec2(0.f, m_PixelsPerMeter * m_Gravity * body->Mass));

            //body->GenerateDrag(0.02f);

            //body->AddTorque(200.f);

            //Update
            body->Update(m_PhysicsTimeStep);
            body->colliding = false;

            if (body->Pos.y >= 550 || body->Pos.y <= 50)
            {
                body->Velocity.y *= -1;
            }
            if (body->Pos.x <= 50 || body->Pos.x >= 750)
            {
                body->Velocity.x *= -1;
            }
        }

        //Loop trough all rigidbodies, except the last one
        for (int i{}; i <= static_cast<int>(m_RigidBodys.size()) - 1; ++i)
        {
            //Each rigidbody checks all the bodies right of it, to avoid duplicate checking
            for(int j{i + 1}; j < static_cast<int>(m_RigidBodys.size()); ++j)
            {
                CollisionSolver::CollisionData data{};

                if(CollisionSolver::IsColliding(m_RigidBodys[i].get(), m_RigidBodys[j].get(), data))
                {
                    m_RigidBodys[i]->colliding = true;
                    m_RigidBodys[j]->colliding = true;
                }
            }
        }

        m_DeltaLag -= m_PhysicsTimeStep;
    }
}

void PhysicsEngine::Draw() const
{
    //Clear render buffer and set background color
    SDL_SetRenderDrawColor(m_pRenderer, 0, 0, 0, 0);
    SDL_RenderClear(m_pRenderer);

    //Set the render color of drawed objects
    //SDL_SetRenderDrawColor(m_pRenderer, 255, 255, 255, 255);
    for (const auto& body : m_RigidBodys)
    {
        body->Draw(m_pRenderer);
    }


    m_FontFPS->Draw();

    //Push the buffer
    SDL_RenderPresent(m_pRenderer);
}