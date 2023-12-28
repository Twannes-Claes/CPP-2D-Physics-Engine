#pragma once
#include "Shape.h"

class Circle final : public Shape
{
public:

	explicit Circle(float radius);

	virtual ~Circle() override = default;

	virtual void UpdateVertices() override {}
	virtual void DrawShape(SDL_Renderer* pRenderer) override;

	bool IsColliding(RigidBody* other) override;

	virtual std::unique_ptr<Shape> Clone() const override;

	virtual Type GetType()  const override { return Type::Circle; }

	float GetMomentOfInteria(const float mass) const override;

	Circle(const Circle& other) = delete;
	Circle(Circle&& other) = delete;
	Circle& operator=(const Circle& other) = delete;
	Circle& operator=(Circle&& other) = delete;

private:

	bool IsCircleColliding(const Circle* other) const;

	float m_Radius{};

	int m_AmountVertices{32};
	float m_AngleBetweenVertices{};

	glm::vec2 xy1{};
	glm::vec2 xy2{};
	glm::vec2 angles{};
};

