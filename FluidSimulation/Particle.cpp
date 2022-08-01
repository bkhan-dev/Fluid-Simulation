#include "Particle.h"

Particle::Particle()
{
}


Particle::~Particle()
{
}

void Particle::addForce(float m)
{
	// calculate acceleration of netforce and add on gravity acceleration
	acceleration += (netForce / m) + glm::vec3(0, -9.8f, 0);
	//acceleration +=  glm::vec3(0, -0.1f, 0);
}

void Particle::update(float deltaTime, float radius, float impulse)
{
	/* Leap Frog */
	// intermediate velocity
	velocity = velocity + (acceleration * (deltaTime / 2));

	// use intermediate velocity to update position
	position = position + (velocity * deltaTime);

	// update velocity for full time step
	velocity = velocity + (acceleration * (deltaTime / 2));

	// collision
	if (position.y - radius < 0) {
		position.y = 0.0 + radius;
		velocity = velocity - (impulse * (glm::dot(velocity, fn))) * fn;

	}


	if ((position.x - radius) < -2.5f) {
		position.x = (-2.5f + radius);
		velocity = velocity - (impulse * (glm::dot(velocity, ln))) * ln;
	}

	if (position.x + radius > 2.5f) {
		position.x = 2.5f - radius;
		velocity = velocity - (impulse * (glm::dot(velocity, rn))) * rn;
	}

	acceleration = glm::vec3(0, 0, 0);
	//// Explicit Euler
	//glm::vec3 oldV = velocity;
	//velocity += acceleration * deltaTime;

	//// pre offset
	//glm::vec3 nextPos = position + (0.5f * (oldV + velocity) * deltaTime);
	//if (nextPos.y < 0) {
	//	nextPos.y = 0.1;
	//	velocity = velocity - (impulse * (glm::dot(velocity, fn))) * fn;
	//	
	//}

	//if (nextPos.x < -2.5f) {
	//	nextPos.x = -2.49f;
	//	velocity = velocity - (impulse * (glm::dot(velocity, ln))) * ln;
	//}

	//if (nextPos.x > 2.5f) {
	//	nextPos.x = 2.49f;
	//	velocity = velocity - (impulse * (glm::dot(velocity, rn))) * rn;
	//}

	//position = nextPos;

}

bool Particle::operator==(const Particle& other) const
{
	return position.x == other.position.x && position.y == other.position.y;
}