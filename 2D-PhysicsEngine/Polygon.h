#pragma once
#include <vector>

#include "Shape.h"

class RigidBody;

class Polygon : public Shape
{
public:

	explicit Polygon(std::vector<SDL_FPoint> vertices);
	virtual ~Polygon() override = default;

	virtual void UpdateVertices() override;
	virtual void DrawShape(SDL_Renderer* pRenderer) override;

	virtual std::unique_ptr<Shape> Clone() const override;

	virtual Type GetType()  const override { return Type::Polygon; }

	float GetMomentOfInteria(const float) const override { return 1000.f; }

	const std::vector<SDL_FPoint>& GetVertices() const { return m_TransformedPoints; }

	glm::vec2 GetEdge(const uint32_t index) const;

	Polygon(const Polygon& other) = delete;
	Polygon(Polygon&& other) = delete;
	Polygon& operator=(const Polygon& other) = delete;
	Polygon& operator=(Polygon&& other) = delete;

private:

	const std::vector<SDL_FPoint> m_Vertices{};
	std::vector<SDL_FPoint> m_TransformedPoints{};

};

