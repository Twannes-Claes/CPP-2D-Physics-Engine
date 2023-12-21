#include "Circle.h"

Circle::Circle(const float radius) :m_Radius(radius) {}

std::unique_ptr<Shape> Circle::Clone() const
{
	return std::make_unique<Circle>(m_Radius);
}

void Circle::DrawShape(SDL_Renderer* pRenderer) const
{
	pRenderer;
}
