#include "PhysicsEngine.h"

//Core includes
#include <chrono>
#include "../Core/Font.h"
#include "../SDL2/include/SDL.h"
#include <string>

//Physics Includes
#include "../PhysicsWorld.h"
#include "../Physics/Circle.h"
#include "../Physics/Box.h"
#include "../Physics/Polygon.h"
#include "../glm/trigonometric.hpp"

//#define MOVE_OBJECTS_MOUSE

PhysicsEngine::PhysicsEngine(const int windowWidth, const int windowHeight, const float physicsTimeStep)
//Initialize the physicsTimeStep
:m_PhysicsTimeStep(physicsTimeStep)
{
    SetupRendering(windowWidth, windowHeight);

    SetupPhysics(windowWidth, windowHeight);
}

//Default assignment is needed in CPP for unique pointers
PhysicsEngine::~PhysicsEngine() = default;

void PhysicsEngine::Run()
{
    //Variables needed for update loop, reference for SDL input and condition if loop has ended
    SDL_Event event;
    bool quitLoop = false;

    //Get the start time
    auto startTime = std::chrono::high_resolution_clock::now();
    auto currTime = startTime;
    float delta{};

    //If quit is true exit the game loop
    while (!quitLoop)
    {
        //Get SDL events
       while (SDL_PollEvent(&event) != 0)
       {
           const SDL_Keycode key = event.key.keysym.sym;

	       switch(event.type)
	       {
	           case SDL_QUIT:
		       {
	           	  quitLoop = true;
		       }
	           break;

               case SDL_KEYDOWN:
               {
	               switch(key)
                   {
                        case SDLK_ESCAPE:
	                    {
                        	quitLoop = true;
	                    }
                        break;
						case SDLK_UP:
	                    {
                            m_IndexCurr = (m_IndexCurr + 1) % m_PhysicsWorld->AmountBodies();
	                    }
                        break;
                        default:
                            break;
                   }
	           	   break;
               }
               break;

	           case SDL_MOUSEBUTTONDOWN:
		       {
                   SDL_GetMouseState(&m_MouseX, &m_MouseY);

                   //When ctrl is pressed spawn static objects
                   //Static objects have 0 mass, so if ctrl is pressed mass is 0
                   //https://wiki.libsdl.org/SDL2/SDL_Keymod
                   const float mass = SDL_GetModState() & KMOD_CTRL /*(KMOD_LCTRL | KMOD_RCTRL)*/ ? 0.f : 1.f;

                   switch (event.button.button )
                   {
                       case SDL_BUTTON_LEFT:
	                   {
                           m_PhysicsWorld->AddBody(Circle(15.f), static_cast<float>(m_MouseX), static_cast<float>(m_MouseY), mass, 0.5f);
	                   }
                       break;
                       case SDL_BUTTON_RIGHT:
	                   {
                       	  constexpr float size = 25.f;
                          m_PhysicsWorld->AddBody(Box(size, size), m_MouseX, m_MouseY, mass, 0.1f, 0.2f);
	                   }
                       break;
                       case SDL_BUTTON_MIDDLE:
                       {
                           const float radius = Random(20.f, 50.f);
                           m_PhysicsWorld->AddBody(Polygon(GenerateConvexPolygon(Random(6, 12), radius), radius), m_MouseX, m_MouseY, mass);
                       }
                       break;
						default:
                            break;
                   }
		       }
	       	   break;

               case SDL_MOUSEMOTION:
               {
	               const auto body = m_PhysicsWorld->GetBodyAtIndex(m_IndexCurr);
                   if (body->IsStatic()) break;

                   //int x, y;
                   SDL_GetMouseState(&m_MouseX, &m_MouseY);

                   body->Pos.x = static_cast<float>(m_MouseX);
                   body->Pos.y = static_cast<float>(m_MouseY);
               }
               break;

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
            UpdateText();
        }

		#ifdef MOVE_OBJECTS_MOUSE
        if (const auto body = m_PhysicsWorld->GetBodyAtIndex(m_IndexCurr); !body->IsStatic())
        {
            SDL_GetMouseState(&m_MouseX, &m_MouseY);

            body->Pos.x = static_cast<float>(m_MouseX);
            body->Pos.y = static_cast<float>(m_MouseY);
        }
		#endif
    }

    //Delete the SDL window/renderer
    SDL_DestroyRenderer(m_pRenderer);
    SDL_DestroyWindow(m_pWindow);

    //Quit SDL
    SDL_Quit();
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

        m_PhysicsWorld->Update(m_PhysicsTimeStep);

        m_DeltaLag -= m_PhysicsTimeStep;

        const auto endFixed = std::chrono::time_point_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now()).time_since_epoch().count();

        m_DurationFixed += static_cast<float>(endFixed - startFixed) * 0.001f;
        ++m_FixedLoopAmount;
    }
}

void PhysicsEngine::Draw() const
{
    //Clear render buffer and set background color
    SDL_SetRenderDrawColor(m_pRenderer, 0, 0, 0, 0);
    SDL_RenderClear(m_pRenderer);

    m_PhysicsWorld->Draw(m_pRenderer);

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
        angle += static_cast<float>((rand() % 60 - 30)) * (static_cast<float>(M_PI) / 180.f);

        const float x = radius * std::cos(angle);
        const float y = radius * std::sin(angle);

        points.push_back({ x, y });
    }

    return points;
}

void PhysicsEngine::SetupRendering(const int w, const int h)
{
    //Initialize SDL
    SDL_Init(SDL_INIT_VIDEO);

    //Create the window
    m_pWindow = SDL_CreateWindow("TwannesClaes-CustomPhysicsEngine", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, w, h, 0);
    m_pRenderer = SDL_CreateRenderer(m_pWindow, -1, SDL_RENDERER_ACCELERATED);

    //Disable VSYNC
    SDL_RenderSetVSync(m_pRenderer, 0);

    const char* fontName{ "Resources/Burbank Big Condensed Black.otf" };

    m_FontFPS = std::make_unique<Font>(fontName, 24, SDL_Color{ 255, 255, 255, 255 }, 65, 10, m_pRenderer);
    m_FontFPSFixed = std::make_unique<Font>(fontName, 24, SDL_Color{ 255, 0, 0, 255 }, 320, 10, m_pRenderer);
    m_FontAmountBodies = std::make_unique<Font>(fontName, 24, SDL_Color{ 255, 255, 0, 255 }, 650, 10, m_pRenderer);
}

void PhysicsEngine::SetupPhysics(const int w, const int h)
{
    m_PhysicsWorld = std::make_unique<PhysicsWorld>();

    //Static circle in the middle
    m_PhysicsWorld->AddBody(Circle(50.f), 200.f, 300.f, 0.f, 1.f);

    //Sloped box
    m_PhysicsWorld->AddBody(Box(50.f, 50.f), 500.f, 300.f, 0.f, 0.5f, 0.5f, static_cast<float>(M_PI) * 0.45f);

    //Bottom floor
    m_PhysicsWorld->AddBody(Box(w / 2.f, 30.f), w / 2.f, h - 15.f, 0.f, 0.8f, 0.2f);

    //Sidewalls
    m_PhysicsWorld->AddBody(Box(30.f, h / 2.f), 15.f, h / 2.f - 45.f, 0.f, 0.2f, 0.2f);
    m_PhysicsWorld->AddBody(Box(30.f, h / 2.f), w - 15.f, h / 2.f - 45.f, 0.f, 0.2f, 0.2f);
}

void PhysicsEngine::UpdateText()
{
    const std::string fpsText = "fps: " + std::to_string(static_cast<int>(m_FrameCount / m_fpsTimer));
    const std::string milliText = "ms: " + std::to_string(m_DurationFixed / m_FixedLoopAmount);
    const std::string amountText = "amount: " + std::to_string(m_PhysicsWorld->AmountBodies());

    m_FontFPS->SetText(fpsText.c_str());
    m_FontFPSFixed->SetText(milliText.c_str());
    m_FontAmountBodies->SetText(amountText.c_str());

    //Reset timer
    m_FrameCount = 0;
    m_fpsTimer = 0;

    m_DurationFixed = 0;
    m_FixedLoopAmount = 0;
}
