 #pragma once
#include <vector>

#include "Polygon.h"

class Box final : public Polygon
{
public:

	explicit Box(float w, float h);
	explicit Box(const glm::vec2& dimensions);

	virtual ~Box() override = default;

	virtual std::unique_ptr<Shape> Clone() const override;

	virtual glm::vec2 GetEdge(uint32_t index) const override;

	virtual Type GetType()  const override { return Type::Box; }

	float GetMomentOfInteria(float mass) const override;

	Box(const Box& other) = delete;
	Box(Box&& other) = delete;
	Box& operator=(const Box& other) = delete;
	Box& operator=(Box&& other) = delete;

private:

	std::vector<SDL_FPoint> GetBoxVertices(const glm::vec2& dimensions) const;

	const glm::vec2 m_Dimensions{};
};

