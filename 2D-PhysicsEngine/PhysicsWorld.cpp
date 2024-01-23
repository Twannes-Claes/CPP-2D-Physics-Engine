#include "PhysicsWorld.h"
#include "SDL2/include/SDL_render.h"

PhysicsWorld::~PhysicsWorld() = default;

//TODO Spatial partiationing, Constraints, Multiple Contact points, Fix Circle vs Poly inside bug

void PhysicsWorld::Update(const float timeStep)
{
    for (const auto& body : m_RigidBodies)
    {
        //Adding the gravity weight force
        body->AddForce(glm::vec2{ static_cast<float>(m_PixelsPerMeter) * Gravity.x * body->Mass, static_cast<float>(m_PixelsPerMeter)* Gravity.y * body->Mass });

        //Update
        body->Update(timeStep);
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
}

void PhysicsWorld::Draw(SDL_Renderer* pRenderer) const
{
    //Set the render color of drawed objects
    SDL_SetRenderDrawColor(pRenderer, 255, 255, 255, 255);
    for (const auto& body : m_RigidBodies)
    {
        body->Draw(pRenderer);
    }
}

void PhysicsWorld::AddBody(const Shape& colliderShape, const float x, const float y, const float mass, const float restitution, const float friction,
    const float rot)
{
    m_RigidBodies.push_back(std::make_unique<RigidBody>(colliderShape, x, y, mass, restitution, friction, rot ));
}

void PhysicsWorld::AddBody(const Shape& colliderShape, const int x, const int y,  const float mass, const float restitution, const float friction,
    const float rot)
{
    AddBody(colliderShape, static_cast<float>(x), static_cast<float>(y), mass, restitution, friction, rot);
}
