#pragma once
#include "Shape.h"
#include "glm/vec2.hpp"

class Box final : public Shape
{
public:

	explicit Box(const float w, const float h);
	explicit Box(const glm::vec2& dimensions);
	virtual ~Box() override = default;

	virtual Type GetType()  const override { return Type::Box; }
	virtual std::unique_ptr<Shape> Clone() const override;
	virtual void DrawShape(SDL_Renderer* pRenderer) const override;

	Box(const Box& other) = delete;
	Box(Box&& other) = delete;
	Box& operator=(const Box& other) = delete;
	Box& operator=(Box&& other) = delete;

private:

	const glm::vec2 m_Dimensions{};

};

