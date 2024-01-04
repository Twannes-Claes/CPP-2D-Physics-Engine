#include "Polygon.h"

#include "glm/ext/matrix_transform.hpp"

Polygon::Polygon(std::vector<SDL_FPoint> vertices):
m_Vertices(std::move(vertices))
{
    m_TransformedPoints.resize(m_Vertices.size());
}

std::unique_ptr<Shape> Polygon::Clone() const
{
	return std::make_unique<Polygon>(m_Vertices);
}

glm::vec2 Polygon::GetEdge(const uint32_t index) const
{
	const uint32_t nextI = (index + 1) % m_Vertices.size();

    const glm::vec2 first{ m_TransformedPoints[index].x, m_TransformedPoints[index].y };
    const glm::vec2 second{ m_TransformedPoints[nextI].x, m_TransformedPoints[nextI].y };

    return second - first;
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
	SDL_RenderDrawLinesF(pRenderer, m_TransformedPoints.data(), static_cast<int>(m_TransformedPoints.size()));
}
