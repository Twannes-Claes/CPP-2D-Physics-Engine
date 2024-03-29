#pragma once
#include <memory>
#include "SDL_render.h"
#include "../glm/vec2.hpp"

class RigidBody;

class Shape
{
public:

	//Enum class for all the different shapes colliders can be
	enum class Type
	{
		Circle,
		Box,
		Polygon 
	};

	//Constructor of shape can only be accessesd by inherited classes, user cant my a (Shape) object only derived (Circle, Box, Polygon)
	//No constructor for shape, each inherited class has their own constructor
	virtual ~Shape() = default;

	virtual void UpdateVertices() = 0;

	virtual void DrawShape(SDL_Renderer* pRenderer) = 0;

	inline virtual float GetMomentOfInteria(float mass) const = 0;

	inline virtual Type GetType() const = 0;

	virtual std::unique_ptr<Shape> Clone() const = 0;

	float GetBoundingRadius() const { return m_BoundingRadius; }

	void UpdatePosRot(const float rigidBodyRot, const glm::vec2& pos) { m_RigidBodyRot = rigidBodyRot; m_RigidBodyPos = pos; }

	Shape(const Shape& other) = delete;
	Shape(Shape&& other) = delete;
	Shape& operator=(const Shape& other) = delete;
	Shape& operator=(Shape&& other) = delete;

protected:

	explicit Shape(const float boundingRadius) :m_BoundingRadius(boundingRadius) {}

	float m_BoundingRadius{};

	float m_RigidBodyRot{};
	glm::vec2 m_RigidBodyPos{};
};

