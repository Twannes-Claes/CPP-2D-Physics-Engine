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

	//Calculating this, goes way over my math skills
	//https://stackoverflow.com/questions/41592034/computing-tensor-of-inertia-in-2d/41618980#41618980
	float GetMomentOfInteria(const float) const override { return 1000.f; }

	const std::vector<SDL_FPoint>& GetVertices() const { return m_TransformedPoints; }

	virtual glm::vec2 GetEdge(const uint32_t index) const;

	Polygon(const Polygon& other) = delete;
	Polygon(Polygon&& other) = delete;
	Polygon& operator=(const Polygon& other) = delete;
	Polygon& operator=(Polygon&& other) = delete;

protected:

	const std::vector<SDL_FPoint> m_Vertices{};
	std::vector<SDL_FPoint> m_TransformedPoints{};

};

