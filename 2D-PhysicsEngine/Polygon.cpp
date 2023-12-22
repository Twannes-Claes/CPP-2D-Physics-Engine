#include "Polygon.h"

#include <iostream>

#include "RigidBody.h"

Polygon::Polygon(std::vector<SDL_FPoint> vertices) :m_Vertices(std::move(vertices)) {}

std::unique_ptr<Shape> Polygon::Clone() const
{
	return std::make_unique<Polygon>(m_Vertices);
}

void Polygon::DrawShape(SDL_Renderer* pRenderer, const glm::vec2& pos)
{
	for (const SDL_FPoint& point : m_Vertices)
	{
		SDL_FPoint newPoint{ point.x + pos.x, point.y + pos.y };
		m_TransformedPoints.push_back(newPoint);
	}

	SDL_RenderDrawLinesF(pRenderer, m_TransformedPoints.data(), static_cast<int>(m_TransformedPoints.size()));

	m_TransformedPoints.clear();
}