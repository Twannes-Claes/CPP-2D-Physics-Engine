#pragma once
#include <memory>
#include "SDL_render.h"
#include "glm/vec2.hpp"

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

	virtual void Update(const glm::vec2& pos) = 0;
	virtual void DrawShape(SDL_Renderer* pRenderer) = 0;

	virtual std::unique_ptr<Shape> Clone() const = 0;

	virtual float GetMomentOfInteria(const float mass) const = 0;

	virtual Type GetType() const = 0;

	//TODO: Make this dirty flag
	void UpdateRotation(const float rigidBodyRot) { m_RigidBodyRot = rigidBodyRot; }



	Shape(const Shape& other) = delete;
	Shape(Shape&& other) = delete;
	Shape& operator=(const Shape& other) = delete;
	Shape& operator=(Shape&& other) = delete;

protected:

	Shape() = default;

	float m_RigidBodyRot{};
};

