#include "Circle.h"

#pragma warning(push)
#pragma warning(disable: 4201) // Disable warning C4201
#include "RigidBody.h"
#include "glm/gtx/norm.hpp"
#pragma warning(pop)


Circle::Circle(const float radius):
m_Radius(radius)
{
	m_AngleBetweenVertices = 360.f / static_cast<float>(m_AmountVertices);
}

std::unique_ptr<Shape> Circle::Clone() const
{
	return std::make_unique<Circle>(m_Radius);
}

void Circle::DrawShape(SDL_Renderer* pRenderer)
{
    SDL_SetRenderDrawColor(pRenderer, 255, 255, 255, 255);

    const float xRotLine = m_RigidBodyPos.x + m_Radius * glm::cos(0.f + m_RigidBodyRot);
    const float yRotLine = m_RigidBodyPos.y + m_Radius * glm::sin(0.f + m_RigidBodyRot);

    SDL_RenderDrawLineF(pRenderer, m_RigidBodyPos.x, m_RigidBodyPos.y, xRotLine, yRotLine);

    for (int i{}; i < m_AmountVertices; ++i)
    {
        angles.x = glm::radians(static_cast<float>(i) * m_AngleBetweenVertices);

        xy1.x = m_RigidBodyPos.x + m_Radius * glm::cos(angles.x);
        xy1.y = m_RigidBodyPos.y + m_Radius * glm::sin(angles.x);

        angles.y = glm::radians(static_cast<float>(i + 1) * m_AngleBetweenVertices);

        xy2.x = m_RigidBodyPos.x + m_Radius * glm::cos(angles.y);
        xy2.y = m_RigidBodyPos.y + m_Radius * glm::sin(angles.y);

        SDL_RenderDrawLineF(pRenderer, xy1.x, xy1.y, xy2.x, xy2.y);
    }
}

bool Circle::IsColliding(RigidBody* other)
{
    Shape* otherShape = other->GetShape();

	switch (otherShape->GetType())
	{
		case Type::Circle:
	    {
		    const Circle* circleShape = dynamic_cast<Circle*>(otherShape);

            if(IsCircleColliding(circleShape))
            {
                return true;
            }
	    }
		default:
            return false;
            break;
	}
}

float Circle::GetMomentOfInteria(const float mass) const
{
    //I = 0.5f(1/2) * r^2 * mass

    return 0.5f * (m_Radius * m_Radius) * mass;
}

bool Circle::IsCircleColliding(const Circle* other) const
{
    //Get the squared distance between the circles
    const float distance = glm::distance2(this->m_RigidBodyPos, other->m_RigidBodyPos);

    //Sum the radius
    const float circleRadiusSum = this->m_Radius + other->m_Radius;

    //If distance squared is smaller than the radius squared circles are colliding
    return distance <= (circleRadiusSum * circleRadiusSum);
}
