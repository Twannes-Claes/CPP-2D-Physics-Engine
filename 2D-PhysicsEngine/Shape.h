#pragma once
#include <memory>
#include "SDL_render.h"

class Shape
{
public:

	//Enum class for all the different shapes colliders can be
	enum class Type
	{
		Circle,
		Box,
		Poly
	};

	//Shape object can only be made by inherited classes
	//No constructor for shape, each inherited class has their own constructor
	virtual ~Shape() = default;

	virtual Type GetType() const = 0;
	virtual std::unique_ptr<Shape> Clone() const = 0;
	virtual void DrawShape(SDL_Renderer* pRenderer) const = 0;

	Shape(const Shape& other) = delete;
	Shape(Shape&& other) = delete;
	Shape& operator=(const Shape& other) = delete;
	Shape& operator=(Shape&& other) = delete;

protected:

	Shape() = default;
};

