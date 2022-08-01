#pragma once
#include <glm/glm.hpp>

class Particle
{
public:
	Particle();
	~Particle();

	// particle attributes
	glm::vec3 position = glm::vec3(0,0,0);
	glm::vec3 velocity = glm::vec3(0, 0, 0);
	glm::vec3 acceleration = glm::vec3(0, 0, 0);
	float density; // density of particle
	float localPressure; // local pressure
	glm::vec3 netForce = glm::vec3(0,0,0);

	// normals for collision
	glm::vec3 ln = glm::vec3(1, 0, 0); // normal of left side of container
	glm::vec3 rn = glm::vec3(-1, 0, 0); // normal of right side of container
	glm::vec3 fn = glm::vec3(0, 1, 0); // normal of base of container;

	// add a force to this particle
	void addForce(float m);
	//update position of particle using leapfrog scheme
	void update(float deltaTime, float particleRadius, float impulse);
	bool operator==(const Particle& other) const;
};

