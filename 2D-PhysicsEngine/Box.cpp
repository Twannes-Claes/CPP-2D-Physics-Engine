#include "Box.h"

#include "RigidBody.h"

Box::Box(const float w, const float h) :Box(glm::vec2{w,h}) {}

Box::Box(const glm::vec2& dimensions):
Polygon(GetBoxVertices(dimensions)),
m_Dimensions(dimensions)
{
	
}

std::unique_ptr<Shape> Box::Clone() const
{
	return std::make_unique<Box>(m_Dimensions);
}

float Box::GetMomentOfInteria(const float mass) const
{
	//I = 1/12 * (w^2 * h^2) * mass
	constexpr float oneTwelfth{ 1.f / 12.f };

	return oneTwelfth * ((m_Dimensions.x * m_Dimensions.x) + (m_Dimensions.y * m_Dimensions.y)) * mass;
}

std::vector<SDL_FPoint> Box::GetBoxVertices(const glm::vec2& dimensions) const
{
	std::vector<SDL_FPoint> points;

	points.push_back({ -dimensions.x, dimensions.y });
	points.push_back({ dimensions.x, dimensions.y });
	points.push_back({ dimensions.x, -dimensions.y });
	points.push_back({ -dimensions.x, -dimensions.y });
	points.push_back({ -dimensions.x, dimensions.y });

	return points;
}
