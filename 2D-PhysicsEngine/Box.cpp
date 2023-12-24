#include "Box.h"

#include "RigidBody.h"

Box::Box(const float w, const float h) :Box(glm::vec2{w,h}) {}

Box::Box(const glm::vec2& dimensions):
m_Dimensions(dimensions)
{
	m_Rect.w = dimensions.x;
	m_Rect.h = dimensions.y;
}

std::unique_ptr<Shape> Box::Clone() const
{
	return std::make_unique<Box>(m_Dimensions);
}

void Box::DrawShape(SDL_Renderer* pRenderer, const glm::vec2& pos)
{
	m_Rect.x = pos.x - (m_Dimensions.x / 2);
	m_Rect.y = pos.y - (m_Dimensions.y / 2);

	SDL_RenderDrawRectF(pRenderer, &m_Rect);
}

float Box::GetMomentOfInteria(const float mass) const
{
	//I = 1/12 * (w^2 * h^2) * mass

	constexpr float oneTwelfth{ 1.f / 12.f };

	return oneTwelfth * ((m_Dimensions.x * m_Dimensions.x) + (m_Dimensions.y * m_Dimensions.y)) * mass;
}
