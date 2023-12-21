#include "Particle.h"

void Particle::AddForce(const glm::vec2& force)
{
	totalForce += force;
}

void Particle::Update(const float deltaTime)
{
	a = totalForce / mass;

	v += a * deltaTime;

	pos += v * deltaTime;

	totalForce = glm::vec2{};
}
