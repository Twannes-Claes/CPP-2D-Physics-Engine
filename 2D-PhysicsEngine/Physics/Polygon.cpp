#include "Polygon.h"

#include "CollisionSolver.h"
#include "../glm/ext/matrix_transform.hpp"

//#define BROADPHASE_DEBUG

Polygon::Polygon(std::vector<SDL_FPoint> vertices, const float boundingRadius):
Shape(boundingRadius),
m_Vertices(std::move(vertices))
{
    m_TransformedPoints.resize(m_Vertices.size());
}

std::unique_ptr<Shape> Polygon::Clone() const
{
	return std::make_unique<Polygon>(m_Vertices, m_BoundingRadius);
}

float Polygon::GetMomentOfInteria(const float) const
{
	//https://stackoverflow.com/questions/41592034/computing-tensor-of-inertia-in-2d/41618980#41618980
    float totalArea{};
    float totalMass{};

    for(int i{}; i < static_cast<int>(m_Vertices.size()); ++i)
    {
        const uint32_t nextI = (i + 1) % m_Vertices.size();

        const glm::vec2& a = glm::vec2{ m_Vertices[i].x, m_Vertices[i].y };
        const glm::vec2& b = glm::vec2{ m_Vertices[nextI].x, m_Vertices[nextI].y };

        const float crossDistancePerpendicular = abs(CollisionSolver::Cross(a, b));

        totalArea += crossDistancePerpendicular * (glm::dot(a, a) + glm::dot(b, b) + glm::dot(a, b));
        totalMass += crossDistancePerpendicular;
    }

    return totalArea / 6.f / totalMass;
}

glm::vec2 Polygon::GetEdge(const uint32_t index) const
{
	const uint32_t nextI = (index + 1) % m_Vertices.size();

    const glm::vec2 first{ m_TransformedPoints[index].x, m_TransformedPoints[index].y };
    const glm::vec2 second{ m_TransformedPoints[nextI].x, m_TransformedPoints[nextI].y };

    return first - second;
    //return second - first;
}

void Polygon::UpdateVertices()
{
    //Precompute rotation matrix
    const glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), m_RigidBodyRot, glm::vec3(0.0f, 0.0f, 1.0f));

    //Convert from local to world space
    for (int i{}; i < static_cast<int>(m_Vertices.size()); ++i)
    {
        glm::vec4 rotatedPoint = rotationMatrix * glm::vec4(m_Vertices[i].x, m_Vertices[i].y, 0.0f, 1.0f);

        const glm::vec2 translatedPoint = glm::vec2(rotatedPoint) + m_RigidBodyPos;

        m_TransformedPoints[i] = { translatedPoint.x, translatedPoint.y };
    }
}

void Polygon::DrawShape(SDL_Renderer* pRenderer)
{
    SDL_RenderDrawPointF(pRenderer, m_RigidBodyPos.x, m_RigidBodyPos.y);
	SDL_RenderDrawLinesF(pRenderer, m_TransformedPoints.data(), static_cast<int>(m_TransformedPoints.size()));
	SDL_RenderDrawLineF(pRenderer, m_TransformedPoints.back().x, m_TransformedPoints.back().y, m_TransformedPoints.front().x, m_TransformedPoints.front().y);

#ifdef BROADPHASE_DEBUG
    //Draw debug bounding circle
    glm::vec2 xy1{};
    glm::vec2 xy2{};
    glm::vec2 angles{};

    constexpr float angleBetween = 360.f / 32.f;

    for (int i{}; i < 32; ++i)
    {
        angles.x = glm::radians(static_cast<float>(i) * angleBetween);

        xy1.x = m_RigidBodyPos.x + m_BoundingRadius * glm::cos(angles.x);
        xy1.y = m_RigidBodyPos.y + m_BoundingRadius * glm::sin(angles.x);

        angles.y = glm::radians(static_cast<float>(i + 1) * angleBetween);

        xy2.x = m_RigidBodyPos.x + m_BoundingRadius * glm::cos(angles.y);
        xy2.y = m_RigidBodyPos.y + m_BoundingRadius * glm::sin(angles.y);

        SDL_RenderDrawLineF(pRenderer, xy1.x, xy1.y, xy2.x, xy2.y);
    }
#endif
}
