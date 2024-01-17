#include "PhysicsEngine.h"

#include <chrono>
#include <iostream>
#include <random>
#include <string>

#include "../SDL2/include/SDL.h"
#include "Font.h"

#include "../Physics/CollisionSolver.h"
#include "../Physics/RigidBody.h"

#include "../Physics/Circle.h"
#include "../Physics/Box.h"
#include "../Physics/Polygon.h"
#include "../glm/trigonometric.hpp"

//#define SAVE_FPS_DATA

PhysicsEngine::PhysicsEngine(const int windowWidth, const int windowHeight, const float physicsTimeStep)
//Initialize the physicsTimeStep
:m_PhysicsTimeStep(physicsTimeStep)
{
    //Initialize SDL
    SDL_Init(SDL_INIT_VIDEO);

    //Create the window
    m_pWindow = SDL_CreateWindow("TwannesClaes-CustomPhysicsEngine", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, windowWidth, windowHeight, 0);
    m_pRenderer = SDL_CreateRenderer(m_pWindow, -1, SDL_RENDERER_ACCELERATED);

    //Disable VSYNC
    SDL_RenderSetVSync(m_pRenderer, 0);

    const char* fontName{ "Burbank Big Condensed Black.otf" };

    m_FontFPS = std::make_unique<Font>(fontName, 24, SDL_Color{ 255, 255, 255, 255 }, 65, 10, m_pRenderer);
    m_FontFPSFixed = std::make_unique<Font>(fontName, 24, SDL_Color{ 255, 0, 0, 255 }, 320, 10, m_pRenderer);
    m_FontAmountBodies = std::make_unique<Font>(fontName, 24, SDL_Color{ 255, 255, 0, 255 }, 650, 10, m_pRenderer);

    //Static circle in the middle
    m_RigidBodies.push_back(std::make_unique<RigidBody>(Circle(50.f), 200.f, 300.f, 0.f, 1.f));
    
    //Sloped box
    m_RigidBodies.push_back(std::make_unique<RigidBody>(Box(50.f, 50.f), 500.f, 300.f, 0.f, 0.5f, 0.5f, static_cast<float>(M_PI) * 0.45f));
    
	//Bottom floor
    m_RigidBodies.push_back(std::make_unique<RigidBody>(Box(windowWidth/2.f, 30.f), windowWidth/2.f, windowHeight - 15.f, 0.f, 0.2f, 0.2f));
    
    //Sidewalls
    m_RigidBodies.push_back(std::make_unique<RigidBody>(Box(30.f, windowHeight/2.f), 15.f, windowHeight / 2.f - 45.f, 0.f, 0.2f, 0.2f));
    m_RigidBodies.push_back(std::make_unique<RigidBody>(Box(30.f, windowHeight/2.f), windowWidth - 15.f, windowHeight / 2.f - 45.f, 0.f, 0.2f, 0.2f));

//#pragma region Determinism experiment
//
//    m_RigidBodies.push_back(std::make_unique<RigidBody>(Circle(25.f), 400, 100, 10.f));
//    m_RigidBodies.push_back(std::make_unique<RigidBody>(Circle(25.f), 410, 150, 5.f,0.1f, 0.8f, 20.f));
//    
//    m_RigidBodies.push_back(std::make_unique<RigidBody>(Box(50.f, 25.f), 350, 20, 10.f));
//    m_RigidBodies.push_back(std::make_unique<RigidBody>(Box(20.f, 45.f), 350, 300, 5.f, 0.2f, 0.4f, 10.f));
//
//    std::vector<SDL_FPoint> points1;
//
//    points1.push_back(SDL_FPoint{ 49.0f, 10.0f });
//    points1.push_back(SDL_FPoint{ 11.0f, 49.0f });
//    points1.push_back(SDL_FPoint{ -28.0f, 41.0f });
//    points1.push_back(SDL_FPoint{ -49.0f, -9.0f });
//    points1.push_back(SDL_FPoint{ -26.0f, -43.0f });
//    points1.push_back(SDL_FPoint{ 3.0f, -50.0f });
//
//    std::vector<SDL_FPoint> points2;
//
//    points2.push_back(SDL_FPoint{ 29.0f, 6.0f });
//    points2.push_back(SDL_FPoint{ 11.0f, 28.0f });
//    points2.push_back(SDL_FPoint{ -9.0f, 29.0f });
//    points2.push_back(SDL_FPoint{ -29.0f, 8.0f });
//    points2.push_back(SDL_FPoint{ -27.0f, -13.0f });
//    points2.push_back(SDL_FPoint{ -19.0f, -23.0f });
//    points2.push_back(SDL_FPoint{ 13.0f, -27.0f });
//
//    m_RigidBodies.push_back(std::make_unique<RigidBody>(Polygon(points1, 50.f), 500, 100, 10.f));
//    m_RigidBodies.push_back(std::make_unique<RigidBody>(Polygon(points2, 50.f), 220, 150, 5.f, 0.8f, 0.2f, 70.f));
//
//    //auto test = GenerateConvexPolygon(7, 30.f);
//    //
//    //for (auto point : test)
//    //{
//    //    std::cout << point.x << ',' << point.y << '\n';
//    //}
//
//#pragma endregion

    //Resize list of fps to 3 minutes worth of saving
    m_DurationsFixedData.resize(180);
    m_AmountBodiesData.resize(180);
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
                        case SDLK_LCTRL:
                        {
                            m_IsCtrlPressed = true;
                            break;
                        }
						case SDLK_UP:
	                    {
                            m_IndexCurr = (m_IndexCurr + 1) % m_RigidBodies.size();

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
               case SDL_KEYUP:
               {
                   if (event.key.keysym.sym == SDLK_LCTRL) m_IsCtrlPressed = false;
               }
	           case SDL_MOUSEBUTTONDOWN:
		       {
                    //Setup for mouse movement to spawn objects
                   SDL_GetMouseState(&m_MouseX, &m_MouseY);

                   const float mass = m_IsCtrlPressed ? 0.f : 1.f;

                   switch (event.button.button )
                   {
                       case SDL_BUTTON_LEFT:
	                   {
						  m_RigidBodies.push_back(std::make_unique<RigidBody>(Circle(15.f), static_cast<float>(m_MouseX), static_cast<float>(m_MouseY), mass, 0.5f));
	                   }
                       break;
                       case SDL_BUTTON_RIGHT:
	                   {
                       	  constexpr float size = 25.f;
						  m_RigidBodies.push_back(std::make_unique<RigidBody>(Box(size, size), m_MouseX, m_MouseY, mass, 0.1f, 0.2f));
	                   }
                       break;
                       case SDL_BUTTON_MIDDLE:
                       {
                           //std::vector<SDL_FPoint> points{};
                           //
                           //points.push_back(SDL_FPoint{ 70.f, 60.f });
                           //points.push_back(SDL_FPoint{ -30.f, 40.f });
                           //points.push_back(SDL_FPoint{ -20.f, -60.f });
                           //points.push_back(SDL_FPoint{ 40.f, -30.f });
                           //points.push_back(SDL_FPoint{ 30.f, 10.f });
                           //
                           //m_RigidBodys.push_back(std::make_unique<RigidBody>(Polygon(points), m_MouseX, m_MouseY, 5.f));
                           const float radius = Random(20.f, 50.f);
                           m_RigidBodies.push_back(std::make_unique<RigidBody>(Polygon(GenerateConvexPolygon(Random(6,12), radius),radius), m_MouseX, m_MouseY, mass));
                       }
                       break;
						default:
                            break;
                   }
		       }
	       	   break;
               case SDL_MOUSEMOTION:
               {
                   if (m_RigidBodies[m_IndexCurr]->IsStatic()) break;

                   //int x, y;
                   SDL_GetMouseState(&m_MouseX, &m_MouseY);
                   m_RigidBodies[m_IndexCurr]->Pos.x = static_cast<float>(m_MouseX);
                   m_RigidBodies[m_IndexCurr]->Pos.y = static_cast<float>(m_MouseY);
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
        //NormalUpdate();

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
            const std::string fpsText = "fps: " + std::to_string(static_cast<int>(m_FrameCount / m_fpsTimer));
            const std::string milliText = "ms: " + std::to_string(m_DurationFixed / m_FixedLoopAmount);
            const std::string amountText = "amount: " + std::to_string(m_RigidBodies.size());

            m_FontFPS->SetText(fpsText.c_str());
            m_FontFPSFixed->SetText(milliText.c_str());
            m_FontAmountBodies->SetText(amountText.c_str());

            //Reset timer
            m_FrameCount = 0;
            m_fpsTimer = 0;

			#ifdef SAVE_FPS_DATA
            //Saving FPS data
            m_DurationsFixedData[m_TotalDurationsData] = m_DurationFixed/m_FixedLoopAmount;
            m_AmountBodiesData[m_TotalDurationsData] = m_RigidBodies.size();
            ++m_TotalDurationsData;

            m_DurationFixed = 0;
            m_FixedLoopAmount = 0;
#           endif
        }

        if (m_RigidBodies[m_IndexCurr]->IsStatic() == false)
        {
            //int x, y;
            SDL_GetMouseState(&m_MouseX, &m_MouseY);
            m_RigidBodies[m_IndexCurr]->Pos.x = static_cast<float>(m_MouseX);
            m_RigidBodies[m_IndexCurr]->Pos.y = static_cast<float>(m_MouseY);
        }

        //if (m_FixedTotalTime >= 15.f) quit = true;
    }

    //Delete the SDL window/renderer
    SDL_DestroyRenderer(m_pRenderer);
    SDL_DestroyWindow(m_pWindow);

    //Quit SDL
    SDL_Quit();

    PrintExperimentData();
}

void PhysicsEngine::FixedUpdate()
{
    //Set a max lag amount so simulation doesnt get stuck
    if (m_DeltaLag > m_MaxDeltaLag)
    {
        m_DeltaLag = m_MaxDeltaLag;
    }

    //Fixed update loop that checks for missed timesteps
    while (m_DeltaLag >= m_PhysicsTimeStep)
    {
        const auto startFixed = std::chrono::time_point_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now()).time_since_epoch().count();

        for (const auto& body : m_RigidBodies)
        {
            //Gravity
            body->AddForce(glm::vec2(0.f, m_PixelsPerMeter * m_Gravity * body->Mass));

            //Update
            body->Update(m_PhysicsTimeStep);
            body->colliding = false;
        }

        const int bodiesSize = static_cast<int>(m_RigidBodies.size());

        //Loop trough all rigidbodies, except the last one
        for (int i{}; i <= bodiesSize - 1; ++i)
        {
            //Each rigidbody checks all the bodies right of it, to avoid duplicate checking
            for (int j{ i + 1 }; j < bodiesSize; ++j)
            {
                CollisionSolver::CollisionData data{};

                if (CollisionSolver::IsColliding(m_RigidBodies[i].get(), m_RigidBodies[j].get(), data))
                {
                    m_DataCollision = data;

                    //Change color when collidng - DEBUG
                    m_RigidBodies[i]->colliding = true;
                    m_RigidBodies[j]->colliding = true;
                }
            }
        }

        m_DeltaLag -= m_PhysicsTimeStep;

        const auto endFixed = std::chrono::time_point_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now()).time_since_epoch().count();

        m_DurationFixed += static_cast<float>(endFixed - startFixed) * 0.001f;
        ++m_FixedLoopAmount;

        m_FixedTotalTime += m_PhysicsTimeStep;
    }
}

void PhysicsEngine::NormalUpdate()
{
    for (const auto& body : m_RigidBodies)
    {
        //Gravity
        body->AddForce(glm::vec2(0.f, m_PixelsPerMeter * m_Gravity * body->Mass));

        //Update
        body->Update(m_DeltaTime);
        body->colliding = false;
    }

    const int bodiesSize = static_cast<int>(m_RigidBodies.size());

    //Loop trough all rigidbodies, except the last one
    for (int i{}; i <= bodiesSize - 1; ++i)
    {
        //Each rigidbody checks all the bodies right of it, to avoid duplicate checking
        for (int j{ i + 1 }; j < bodiesSize; ++j)
        {
            CollisionSolver::CollisionData data{};

            if (CollisionSolver::IsColliding(m_RigidBodies[i].get(), m_RigidBodies[j].get(), data))
            {
                m_DataCollision = data;

                //Change color when collidng - DEBUG
                m_RigidBodies[i]->colliding = true;
                m_RigidBodies[j]->colliding = true;
            }
        }
    }

    m_FixedTotalTime += m_DeltaTime;
}

void PhysicsEngine::Draw() const
{
    //Clear render buffer and set background color
    SDL_SetRenderDrawColor(m_pRenderer, 0, 0, 0, 0);
    SDL_RenderClear(m_pRenderer);

    //Set the render color of drawed objects
    //SDL_SetRenderDrawColor(m_pRenderer, 255, 255, 255, 255);
    for (const auto& body : m_RigidBodies)
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

void PhysicsEngine::PrintExperimentData() const
{
	#ifdef SAVE_FPS_DATA
    //FPS DATA
    for (uint32_t i{}; i < m_TotalDurationsData; ++i)
    {
        std::cout << m_DurationsFixedData[i] << " || " << m_AmountBodiesData[i] << '\n';
    }
	#endif

    //uint32_t bodyIndex{};

    ////BODY DATA
    //for (uint32_t i{}; i < m_RigidBodies.size(); ++i)
    //{
    //    const auto& body = m_RigidBodies[i];
    //
    //    if(body->IsStatic()) continue;
    //
    //    ++bodyIndex;
    //
    //    std::cout << bodyIndex << ":\n"
    //	<< body->Pos.x << ", " << body->Pos.y << '\n'
    //    << body->Velocity.x << ", " << body->Velocity.y << '\n'
    //    << body->Rot << '\n';
    //
    //}
}
