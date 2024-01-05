#include "PhysicsEngine.h"

#include <chrono>
#include <iostream>
#include <string>

#include "SDL2/include/SDL.h"
#include "Font.h"

#include "CollisionSolver.h"
#include "RigidBody.h"

#include "Circle.h"
#include "Box.h"
#include "Polygon.h"

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

    //Static circle in the middle
    m_RigidBodys.push_back(std::make_unique<RigidBody>(Circle(50.f), 400.f, 300.f, 1.f, 1.f));

    ////Sloped box
    m_RigidBodys.push_back(std::make_unique<RigidBody>(Box(50.f, 50.f), 400.f, 300.f, 0.f, 0.5f, 0.5f, static_cast<float>(M_PI) * 0.45f));
    //
    //Bottom floor
    m_RigidBodys.push_back(std::make_unique<RigidBody>(Box(800.f, 30.f), 400.f, 580.f, 0.f, 0.2f, 0.2f));
    
    //Sidewalls
    m_RigidBodys.push_back(std::make_unique<RigidBody>(Box(30.f, 600.f), 15.f, 260.f, 0.f, 0.2f, 0.2f));
    m_RigidBodys.push_back(std::make_unique<RigidBody>(Box(30.f, 600.f), 784.f, 260.f, 0.f, 0.2f, 0.2f));

    //https://www.jeffreythompson.org/collision-detection/poly-poly.php
    //Randomly generated polygons
    //// set position of the pentagon's vertices
    //float angle = TWO_PI / pentagon.length;
    //for (int i = 0; i < pentagon.length; i++) {
    //    float a = angle * i;
    //    float x = 300 + cos(a) * 100;
    //    float y = 200 + sin(a) * 100;
    //    pentagon[i] = new PVector(x, y);
    //}
    //
    //// and create the random polygon
    //float a = 0;
    //int i = 0;
    //while (a < 360) {
    //    float x = cos(radians(a)) * random(30, 50);
    //    float y = sin(radians(a)) * random(30, 50);
    //    randomPoly[i] = new PVector(x, y);
    //    a += random(15, 40);
    //    i += 1;
    //}
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
                   int x{}, y{};
                   SDL_GetMouseState(&x, &y);

                   if (event.button.button == SDL_BUTTON_LEFT)
                   {
						m_RigidBodys.push_back(std::make_unique<RigidBody>(Circle(15.f), static_cast<float>(x), static_cast<float>(y), 1.f, 0.5f));
                   }
                   else if(event.button.button == SDL_BUTTON_RIGHT)
                   {
                       constexpr float size = 15.f;
                       m_RigidBodys.push_back(std::make_unique<RigidBody>(Box(size, size), x, y, 1.f, 0.1f, 0.2f));
                   }
		       }
	       	   break;
               case SDL_MOUSEMOTION:
               {
                   int x, y;
                   SDL_GetMouseState(&x, &y);
                   m_RigidBodys[0]->Pos.x = static_cast<float>(x);
                   m_RigidBodys[0]->Pos.y = static_cast<float>(y);
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
        const auto startFixed = std::chrono::time_point_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now()).time_since_epoch().count();

        for (const auto& body : m_RigidBodys)
        {
            //Add wind
            //body->AddForce(glm::vec2{ 20.f, 0.f });

            //Gravity
            body->AddForce(glm::vec2(0.f, m_PixelsPerMeter * m_Gravity * body->Mass));
            
            //body->GenerateDrag(0.002f);

            //body->AddTorque(200.f);

            //Update
            body->Update(m_PhysicsTimeStep);
            body->colliding = false;

            if (body->GetShape()->GetType() == Shape::Type::Circle)
            {
                const Circle* circleShape = dynamic_cast<Circle*>(body->GetShape());

                if (body->Pos.x - circleShape->GetRadius() <= 0)
                {
                    body->Pos.x = circleShape->GetRadius();
                    body->Velocity.x *= -0.9f;
                }
                else if (body->Pos.x + circleShape->GetRadius() >= 800)
                {
                    body->Pos.x = 800 - circleShape->GetRadius();
                    body->Velocity.x *= -0.9f;
                }
                if (body->Pos.y - circleShape->GetRadius() <= 0)
                {
                    body->Pos.y = circleShape->GetRadius();
                    body->Velocity.y *= -0.9f;
                }
                else if (body->Pos.y + circleShape->GetRadius() >= 600)
                {
                    body->Pos.y = 600 - circleShape->GetRadius();
                    body->Velocity.y *= -0.9f;
                }
            }
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

    SDL_SetRenderDrawColor(m_pRenderer, 0, 0, 255, 255);
    //SDL_RenderDrawPointF(m_pRenderer, m_DataCollision.start.x, m_DataCollision.start.y);
    //SDL_RenderDrawPointF(m_pRenderer, m_DataCollision.end.x, m_DataCollision.end.y);

    const SDL_FRect startPoint = SDL_FRect{ m_DataCollision.start.x - 2.f, m_DataCollision.start.y - 2.f, 4.f, 4.f };
    const SDL_FRect endPoint = SDL_FRect{ m_DataCollision.end.x - 2.f, m_DataCollision.end.y - 2.f, 4.f, 4.f };

    SDL_RenderDrawRectF(m_pRenderer, &startPoint);
    SDL_RenderDrawRectF(m_pRenderer, &endPoint);

    //std::cout << m_DataCollision.normal.x << ", " << m_DataCollision.normal.x << '\n';

    SDL_RenderDrawLineF(m_pRenderer, m_DataCollision.start.x, m_DataCollision.start.y, m_DataCollision.start.x + m_DataCollision.normal.x * 15.f, m_DataCollision.start.y + m_DataCollision.normal.y * 15.f);

    m_FontFPS->Draw();
    m_FontFPSFixed->Draw();
    m_FontAmountBodies->Draw();

    //Push the buffer
    SDL_RenderPresent(m_pRenderer);
}