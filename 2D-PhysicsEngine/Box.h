#pragma once
#include <vector>

#include "Polygon.h"
#include "glm/vec2.hpp"

class Box final : public Polygon
{
public:

	explicit Box(const float w, const float h);
	explicit Box(const glm::vec2& dimensions);
	virtual ~Box() override = default;

	virtual void Update(const glm::vec2& pos) override { Polygon::Update(pos); }
	virtual void DrawShape(SDL_Renderer* pRenderer) override { Polygon::DrawShape(pRenderer); }

	virtual std::unique_ptr<Shape> Clone() const override;

	virtual Type GetType()  const override { return Type::Box; }

	float GetMomentOfInteria(const float mass) const override;

	Box(const Box& other) = delete;
	Box(Box&& other) = delete;
	Box& operator=(const Box& other) = delete;
	Box& operator=(Box&& other) = delete;

private:

	std::vector<SDL_FPoint> GetBoxVertices(const glm::vec2& dimensions) const;

	const glm::vec2 m_Dimensions{};
};

