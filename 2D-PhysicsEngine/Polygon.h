#pragma once
#include <vector>

#include "Shape.h"
#include "glm/vec2.hpp"

class Polygon final : public Shape
{
public:

	explicit Polygon(std::vector<glm::vec2> vertices);
	virtual ~Polygon() override = default;

	virtual Type GetType()  const override { return Type::Poly; }
	virtual std::unique_ptr<Shape> Clone() const override;
	virtual void DrawShape(SDL_Renderer* pRenderer) const override;

	Polygon(const Polygon& other) = delete;
	Polygon(Polygon&& other) = delete;
	Polygon& operator=(const Polygon& other) = delete;
	Polygon& operator=(Polygon&& other) = delete;

private:

	const std::vector<glm::vec2> m_Vertices{};

};

