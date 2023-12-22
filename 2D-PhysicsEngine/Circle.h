#pragma once
#include "Shape.h"

class Circle final : public Shape
{
public:

	explicit Circle(const float radius);
	virtual ~Circle() override = default;

	virtual Type GetType()  const override { return Type::Circle; }
	virtual std::unique_ptr<Shape> Clone() const override;
	virtual void DrawShape(SDL_Renderer* pRenderer, const glm::vec2& pos) override;

	Circle(const Circle& other) = delete;
	Circle(Circle&& other) = delete;
	Circle& operator=(const Circle& other) = delete;
	Circle& operator=(Circle&& other) = delete;

private:

	//int DrawLine(SDL_Renderer* renderer, const float x1, const float x2, const float y) const;
	//int DrawPoint(SDL_Renderer* renderer, const float x, const float y) const;
	//
	//int DrawCircle(SDL_Renderer* renderer, float x, float y, int rx, int ry) const;

	float m_Radius{};
	int m_AmountVertices{24};
	float m_AngleBetweenVertices{};

	glm::vec2 xy1{};
	glm::vec2 xy2{};
	glm::vec2 angles{};

};

