#pragma once

#include "glm/vec2.hpp"

class Particle
{
public:

	Particle(float x, float y, float mass): pos(x,y), mass(mass){}
	~Particle() = default;

	Particle(const Particle& other) = delete;
	Particle(Particle&& other) = delete;
	Particle& operator=(const Particle& other) = delete;
	Particle& operator=(Particle&& other) = delete;

	void AddForce(const glm::vec2& force);

	void Update(const float deltaTime);

	glm::vec2 pos{};
	glm::vec2 v{};
	glm::vec2 a{};
	float mass{};

private:

	glm::vec2 totalForce{};



};

