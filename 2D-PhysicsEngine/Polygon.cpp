#include "Polygon.h"

Polygon::Polygon(std::vector<glm::vec2> vertices) :m_Vertices(std::move(vertices)) {}

std::unique_ptr<Shape> Polygon::Clone() const
{
	return std::make_unique<Polygon>(m_Vertices);
}

void Polygon::DrawShape(SDL_Renderer* pRenderer) const
{
	pRenderer;
}