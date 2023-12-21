#include "Box.h"

Box::Box(const float w, const float h) :Box(glm::vec2{w,h}) {}

Box::Box(const glm::vec2& dimensions) :m_Dimensions(dimensions) {}

std::unique_ptr<Shape> Box::Clone() const
{
	return std::make_unique<Box>(m_Dimensions);
}

void Box::DrawShape(SDL_Renderer* pRenderer) const
{
	pRenderer;
}
