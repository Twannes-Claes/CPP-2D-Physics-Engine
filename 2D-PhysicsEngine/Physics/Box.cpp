#include "Box.h"

#include "../glm/ext/quaternion_geometric.hpp"

Box::Box(const float w, const float h) :Box(glm::vec2{w,h}) {}

Box::Box(const glm::vec2& dimensions):
Polygon(GetBoxVertices(dimensions), length(dimensions)),
m_Dimensions(dimensions)
{}

std::unique_ptr<Shape> Box::Clone() const
{
	return std::make_unique<Box>(m_Dimensions);
}

glm::vec2 Box::GetEdge(const uint32_t index) const
{
	const uint32_t nextI = (index + 1) % m_Vertices.size();

	const glm::vec2 first{ m_TransformedPoints[index].x, m_TransformedPoints[index].y };
	const glm::vec2 second{ m_TransformedPoints[nextI].x, m_TransformedPoints[nextI].y };

	return second - first;
}

float Box::GetMomentOfInteria(const float mass) const
{
	//I = 1/12 * (w^2 * h^2) * mass
	constexpr float oneTwelfth{ 1.f / 12.f };

	return oneTwelfth * ((m_Dimensions.x * m_Dimensions.x) + (m_Dimensions.y * m_Dimensions.y)) * mass;
}

std::vector<SDL_FPoint> Box::GetBoxVertices(const glm::vec2& dimensions) const
{
	//If local vertices are already assigned return those
	//if (!m_Vertices.empty()) return m_Vertices;

	std::vector<SDL_FPoint> points;

	points.push_back({ -dimensions.x, dimensions.y });
	points.push_back({ dimensions.x, dimensions.y });
	points.push_back({ dimensions.x, -dimensions.y });
	points.push_back({ -dimensions.x, -dimensions.y });

	return points;
}
