#include "PhysicsEngine.h"

#include <chrono>
#include <iostream>
#include <random>
#include <string>

#include "SDL2/include/SDL.h"
#include "Font.h"

#include "CollisionSolver.h"
#include "RigidBody.h"

#include "Circle.h"
#include "Box.h"
#include "Polygon.h"
#include "glm/trigonometric.hpp"

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

    const char* fontName{ "Burbank Big Condensed Black.otf" };

    m_FontFPS = std::make_unique<Font>(fontName, 24, SDL_Color{ 255, 255, 255, 255 }, 10, 10, m_pRenderer);
    m_FontFPSFixed = std::make_unique<Font>(fontName, 24, SDL_Color{ 255, 0, 0, 255 }, 100, 10, m_pRenderer);
    m_FontAmountBodies = std::make_unique<Font>(fontName, 24, SDL_Color{ 255, 255, 0, 255 }, 750, 10, m_pRenderer);

    std::vector<SDL_FPoint> points{};

    points.push_back(SDL_FPoint{ 20.f, 60.f });
    points.push_back(SDL_FPoint{ -40.f, 20.f });
    points.push_back(SDL_FPoint{ -20.f, -60.f });
    points.push_back(SDL_FPoint{ 20.f, -60.f });
    points.push_back(SDL_FPoint{ 40.f, 20.f });

    //m_RigidBodys.push_back(std::make_unique<RigidBody>(Polygon(points), 10, 500, 5.f));
    //m_RigidBodys.push_back(std::make_unique<RigidBody>(Polygon(points), 400, 500, 5.f));
    //m_RigidBodys.push_back(std::make_unique<RigidBody>(Polygon(GenerateConvexPolygon(Random(6, 8), 40.f)), 10, 500, 5.f));
    //m_RigidBodys.push_back(std::make_unique<RigidBody>(Polygon(GenerateConvexPolygon(Random(6, 8), 30.f)), 400, 300, 1.f));

    //Static circle in the middle
    m_RigidBodys.push_back(std::make_unique<RigidBody>(Circle(50.f), 200.f, 300.f, 0.f, 1.f));
    
    //Sloped box
    m_RigidBodys.push_back(std::make_unique<RigidBody>(Box(50.f, 50.f), 500.f, 300.f, 0.f, 0.5f, 0.5f, static_cast<float>(M_PI) * 0.45f));
    
	//Bottom floor
    m_RigidBodys.push_back(std::make_unique<RigidBody>(Box(windowWidth/2.f, 30.f), windowWidth/2.f, windowHeight - 15.f, 0.f, 0.2f, 0.2f));
    
    //Sidewalls
    m_RigidBodys.push_back(std::make_unique<RigidBody>(Box(30.f, windowHeight/2.f), 15.f, windowHeight / 2.f - 45.f, 0.f, 0.2f, 0.2f));
    m_RigidBodys.push_back(std::make_unique<RigidBody>(Box(30.f, windowHeight/2.f), windowWidth - 15.f, windowHeight / 2.f - 45.f, 0.f, 0.2f, 0.2f));
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
						case SDLK_UP:
	                    {
                            m_IndexCurr = (m_IndexCurr + 1) % m_RigidBodys.size();

                            //while(m_RigidBodys[m_IndexCurr]->IsStatic())
                            //{
                            //    m_IndexCurr = (m_IndexCurr + 1) % m_RigidBodys.size();
                            //}

                            break;
	                    }
                        default:
                            break;
                   }
	           	   break;
               }
	           case SDL_MOUSEBUTTONDOWN:
		       {
                    //Setup for mouse movement to spawn objects
                   SDL_GetMouseState(&m_MouseX, &m_MouseY);

                   switch (event.button.button )
                   {
                       case SDL_BUTTON_LEFT:
	                   {
						  m_RigidBodys.push_back(std::make_unique<RigidBody>(Circle(15.f), static_cast<float>(m_MouseX), static_cast<float>(m_MouseY), 1.f, 0.5f));
	                   }
                       break;
                       case SDL_BUTTON_RIGHT:
	                   {
                       	  constexpr float size = 25.f;
						  m_RigidBodys.push_back(std::make_unique<RigidBody>(Box(size, size), m_MouseX, m_MouseY, 1.f, 0.1f, 0.2f));
	                   }
                       break;
                       case SDL_BUTTON_MIDDLE:
                       {
                           //std::vector<SDL_FPoint> points{};
                           //
                           //points.push_back(SDL_FPoint{ 20.f, 60.f });
                           //points.push_back(SDL_FPoint{ -40.f, 20.f });
                           //points.push_back(SDL_FPoint{ -20.f, -60.f });
                           //points.push_back(SDL_FPoint{ 20.f, -60.f });
                           //points.push_back(SDL_FPoint{ 40.f, 20.f });
                           //
                           //m_RigidBodys.push_back(std::make_unique<RigidBody>(Polygon(points), m_MouseX, m_MouseY, 5.f));
                           const float radius = Random(20.f, 50.f);
                           m_RigidBodys.push_back(std::make_unique<RigidBody>(Polygon(GenerateConvexPolygon(Random(6,8), radius),radius), m_MouseX, m_MouseY, 1.f));
                       }
                       break;
						default:
                            break;
                   }
		       }
	       	   break;
               case SDL_MOUSEMOTION:
               {
                   if (m_RigidBodys[m_IndexCurr]->IsStatic()) break;

                   //int x, y;
                   SDL_GetMouseState(&m_MouseX, &m_MouseY);
                   m_RigidBodys[m_IndexCurr]->Pos.x = static_cast<float>(m_MouseX);
                   m_RigidBodys[m_IndexCurr]->Pos.y = static_cast<float>(m_MouseY);
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

        m_fpsTimer += delta;
        ++m_FrameCount;

        // Calculate average FPS every second
        if (m_fpsTimer >= 1.f)
        {
            m_FontFPS->SetText(std::to_string(static_cast<int>(m_FrameCount/ m_fpsTimer)).c_str());
            m_FontFPSFixed->SetText(std::to_string(m_DurationFixed).c_str());
            m_FontAmountBodies->SetText(std::to_string(m_RigidBodys.size()).c_str());

            //Reset timer
            m_FrameCount = 0;
            m_fpsTimer = 0;
        }

        if (m_RigidBodys[m_IndexCurr]->IsStatic() == false)
        {
            //int x, y;
            SDL_GetMouseState(&m_MouseX, &m_MouseY);
            m_RigidBodys[m_IndexCurr]->Pos.x = static_cast<float>(m_MouseX);
            m_RigidBodys[m_IndexCurr]->Pos.y = static_cast<float>(m_MouseY);
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
    if (m_DeltaLag > m_MaxDeltaLag)
    {
        m_DeltaLag = m_MaxDeltaLag;
    }

    //Fixed update loop that checks for missed timesteps
    while (m_DeltaLag >= m_PhysicsTimeStep)
    {
        const auto startFixed = std::chrono::time_point_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now()).time_since_epoch().count();

        for (const auto& body : m_RigidBodys)
        {
            //Gravity
            body->AddForce(glm::vec2(0.f, m_PixelsPerMeter * m_Gravity * body->Mass));

            //Update
            body->Update(m_PhysicsTimeStep);
            body->colliding = false;
        }

        const int bodiesSize = static_cast<int>(m_RigidBodys.size());

        //Loop trough all rigidbodies, except the last one
        for (int i{}; i <= bodiesSize - 1; ++i)
        {
            //Each rigidbody checks all the bodies right of it, to avoid duplicate checking
            for (int j{ i + 1 }; j < bodiesSize; ++j)
            {
                CollisionSolver::CollisionData data{};

                if (CollisionSolver::IsColliding(m_RigidBodys[i].get(), m_RigidBodys[j].get(), data))
                {
                    m_DataCollision = data;

                    //Change color when collidng - DEBUG
                    m_RigidBodys[i]->colliding = true;
                    m_RigidBodys[j]->colliding = true;
                }
            }
        }

        m_DeltaLag -= m_PhysicsTimeStep;

        const auto endFixed = std::chrono::time_point_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now()).time_since_epoch().count();

        m_DurationFixed = static_cast<float>(endFixed - startFixed) * 0.001f;
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

    //SDL_SetRenderDrawColor(m_pRenderer, 0, 0, 255, 255);
    //
    //const SDL_FRect startPoint = SDL_FRect{ m_DataCollision.start.x - 2.f, m_DataCollision.start.y - 2.f, 4.f, 4.f };
    //const SDL_FRect endPoint = SDL_FRect{ m_DataCollision.end.x - 2.f, m_DataCollision.end.y - 2.f, 4.f, 4.f };
    //
    //SDL_RenderDrawRectF(m_pRenderer, &startPoint);
    //SDL_RenderDrawRectF(m_pRenderer, &endPoint);
    //
    //SDL_RenderDrawLineF(m_pRenderer, m_DataCollision.start.x, m_DataCollision.start.y, m_DataCollision.start.x + m_DataCollision.normal.x * 15.f, m_DataCollision.start.y + m_DataCollision.normal.y * 15.f);

    m_FontFPS->Draw();
    m_FontFPSFixed->Draw();
    m_FontAmountBodies->Draw();

    //Push the buffer
    SDL_RenderPresent(m_pRenderer);
}

float PhysicsEngine::Random(const float min, const float max) const
{
    return min + static_cast<float>(rand()) / RAND_MAX * (max - min);
}

int PhysicsEngine::Random(const int min, const int max) const
{
    return min + rand() / RAND_MAX * (max - min);
}

std::vector<SDL_FPoint> PhysicsEngine::GenerateConvexPolygon(const int numVertices, const float radius) const
{
    std::vector<SDL_FPoint> points;

    for (int i = 0; i < numVertices; ++i) 
    {
        float angle = static_cast<float>(i) * (2.f * static_cast<float>(M_PI)) / static_cast<float>(numVertices);
        angle += (rand() % 60 - 30) * (static_cast<float>(M_PI) / 180.f);

        const float x = radius * std::cos(angle);
        const float y = radius * std::sin(angle);

        points.push_back({ x, y });
    }

    return points;
}
