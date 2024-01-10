#include "Circle.h"

#pragma warning(push)
#pragma warning(disable: 4201) // Disable warning C4201
#include "glm/gtx/norm.hpp"
#pragma warning(pop)


Circle::Circle(const float radius):
Shape(radius)
{
	m_AngleBetweenVertices = 360.f / static_cast<float>(m_AmountVertices);
}

std::unique_ptr<Shape> Circle::Clone() const
{
	return std::make_unique<Circle>(m_BoundingRadius);
}

void Circle::DrawShape(SDL_Renderer* pRenderer)
{
    const float xRotLine = m_RigidBodyPos.x + m_BoundingRadius * glm::cos(0.f + m_RigidBodyRot);
    const float yRotLine = m_RigidBodyPos.y + m_BoundingRadius * glm::sin(0.f + m_RigidBodyRot);

    SDL_RenderDrawLineF(pRenderer, m_RigidBodyPos.x, m_RigidBodyPos.y, xRotLine, yRotLine);

    for (int i{}; i < m_AmountVertices; ++i)
    {
        angles.x = glm::radians(static_cast<float>(i) * m_AngleBetweenVertices);

        xy1.x = m_RigidBodyPos.x + m_BoundingRadius * glm::cos(angles.x);
        xy1.y = m_RigidBodyPos.y + m_BoundingRadius * glm::sin(angles.x);

        angles.y = glm::radians(static_cast<float>(i + 1) * m_AngleBetweenVertices);

        xy2.x = m_RigidBodyPos.x + m_BoundingRadius * glm::cos(angles.y);
        xy2.y = m_RigidBodyPos.y + m_BoundingRadius * glm::sin(angles.y);

        SDL_RenderDrawLineF(pRenderer, xy1.x, xy1.y, xy2.x, xy2.y);
    }
}

float Circle::GetMomentOfInteria(const float mass) const
{
    //I = 0.5f(1/2) * r^2 * mass
    return 0.5f * (m_BoundingRadius * m_BoundingRadius) * mass;
}
