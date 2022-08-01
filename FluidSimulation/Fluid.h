#pragma once
#define GLEW_STATIC
#include <GL/glew.h> 
#include <vector>
#include <iostream>
#include "Particle.h"
#include <math.h>
#include "Shader.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


class Fluid
{
public:
	Fluid();
	~Fluid();

	std::vector<Particle> particles;
	std::vector<glm::vec3> circleVertices;

	void update(float deltaTime);
	void render(Shader _shader);

	bool spikyKernelOn = true;
	bool poly6KernelOn = false;
	bool viscosityOn = true;
	bool renderPointParticles = false;

	float pi = 3.14159265358979323846f;
	float p0 = 1.0f; // rest density
	float h = 0.27f; // support radius for smoothing kernel
	float M = 1.0f; // mass of each particle is the same
	float k = 3.0; // gas constant
	float u = 0.9f; // viscosity 
	float iForce = 1.01f; // impulse force


	float particleRadius = 0.2f;

	void calculateParticles();
	void reset();

private:
	unsigned int VAO, VBO; // vao/vbo for particles ( if rendering points )
	unsigned int mVAO, mVBO; // vao/vbo for discs 
	void updateData(); // updates the buffer data

	float calculateDensity(Particle ri); // calculates density using poly6 kernel 
	glm::vec3 calculatePressure(Particle ri); // Calculates pressure using poly6
	glm::vec3 calculatePressureSpiky(Particle ri); // Calculates pressure using spiky kernel
	glm::vec3 calculateViscosity(Particle ri); // Calculates the viscosity using viscosity kernel 

	void calculateCircleVertices(); // draws a disc at each particle position
};

