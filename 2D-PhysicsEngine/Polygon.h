#pragma once
#include <vector>

#include "Shape.h"
#include "glm/vec2.hpp"

class RigidBody;

class Polygon final : public Shape
{
public:

	explicit Polygon(std::vector<SDL_FPoint> vertices);
	virtual ~Polygon() override = default;

	virtual Type GetType()  const override { return Type::Polygon; }
	virtual std::unique_ptr<Shape> Clone() const override;
	virtual void DrawShape(SDL_Renderer* pRenderer, const glm::vec2& pos) override;

	Polygon(const Polygon& other) = delete;
	Polygon(Polygon&& other) = delete;
	Polygon& operator=(const Polygon& other) = delete;
	Polygon& operator=(Polygon&& other) = delete;

private:

	const std::vector<SDL_FPoint> m_Vertices{};
	std::vector<SDL_FPoint> m_TransformedPoints{};

};

