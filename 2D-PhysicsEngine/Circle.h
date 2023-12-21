#pragma once
#include "Shape.h"

class Circle final : public Shape
{
public:

	explicit Circle(const float radius);
	virtual ~Circle() override = default;

	virtual Type GetType()  const override { return Type::Circle; }
	virtual std::unique_ptr<Shape> Clone() const override;
	virtual void DrawShape(SDL_Renderer* pRenderer) const override;

	Circle(const Circle& other) = delete;
	Circle(Circle&& other) = delete;
	Circle& operator=(const Circle& other) = delete;
	Circle& operator=(Circle&& other) = delete;

private:

	float m_Radius{};
};

