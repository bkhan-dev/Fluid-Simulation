#include "Fluid.h"

Fluid::Fluid()
{
	calculateParticles();
	calculateCircleVertices();
	std::cout << particles.size();

	glGenVertexArrays(1, &mVAO);
	glGenBuffers(1, &mVBO);

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	// particle point setup 
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, particles.size() * sizeof(particles[0]), &particles[0], GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 23 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);

}


Fluid::~Fluid()
{
}

void Fluid::update(float deltaTime)
{
	//// density and local pressure for each particle
	for (int i = 0; i < particles.size(); i++) {
		// set netforces to 0
		particles[i].netForce = glm::vec3(0, 0, 0);
		// compute density for this particle
		particles[i].density = calculateDensity(particles[i]);
		// also compute its local pressure
		particles[i].localPressure = k * (particles[i].density - p0);
	}

	// pressure force for each particle
	glm::vec3 pressureForce;
	glm::vec3 viscosityForce = glm::vec3(0,0,0);
	for (size_t i = 0; i < particles.size(); i++){
		// compute the pressure force
		if (poly6KernelOn)
		{
			pressureForce = calculatePressure(particles[i]);
		}
		else if(spikyKernelOn)
		{
			pressureForce = calculatePressureSpiky(particles[i]);
		}

		if (viscosityOn) {
			viscosityForce = calculateViscosity(particles[i]);
		}
		// add the force to the netforce of this particle
		particles[i].netForce = pressureForce + viscosityForce;
	}

	// gravity for each particle
	for (int i = 0; i < particles.size(); i++) {
		// now calculate acceleration of each particle using their netforces
		particles[i].addForce(M);
	}

	// update position and velocities for each particle
	for (int i = 0; i < particles.size(); i++) {
		// update each particle
		particles[i].update(deltaTime, particleRadius, iForce);
	}

	updateData();

}

void Fluid::render(Shader _shader)
{
	if (renderPointParticles)
	{
		glBindVertexArray(VAO);
		glPointSize(4.0f);
		glDrawArrays(GL_POINTS, 0, particles.size());
		glBindVertexArray(0);
	}
	else {
		// render a disc at every particle position
		glBindVertexArray(mVAO);

		for (size_t i = 0; i < particles.size(); i++)
		{
			glm::mat4 model(1.0f);
			model = glm::translate(model, particles[i].position);
			_shader.setMat4("model", model);

			glBindBuffer(GL_ARRAY_BUFFER, mVBO);
			glBufferData(GL_ARRAY_BUFFER, circleVertices.size() * sizeof(circleVertices[0]), &circleVertices[0], GL_STATIC_DRAW);

			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(0);

			glDrawArrays(GL_TRIANGLE_FAN, 0, circleVertices.size());
		}

		glBindVertexArray(0);
	}

}

void Fluid::calculateParticles()
{
	Particle particle;
	for (float x = -1.5f; x <= 0.5f; x += 0.25f)
	{
		for (float y = 4.0f; y <= 7.0f; y += 0.25f)
		{
			particle.position = glm::vec3(x, y, 0);
			particles.push_back(particle);
		}
	}
}

void Fluid::reset()
{
	particles.clear();
	calculateParticles();
	updateData();
}

void Fluid::updateData()
{
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, particles.size() * sizeof(particles[0]), &particles[0], GL_STATIC_DRAW);
	glBindVertexArray(0);
}

// given particle ri, calculate its density using a weighted average from all other particles
float Fluid::calculateDensity(Particle ri)
{
	// define constants
	float poly6constant = 315.0f / (64.0f * pi * pow(h, 9));

	Particle rj;
	float r;
	float rSquared;
	float hSquared = h * h;

	float density = 0.0f;
	for (size_t j = 0; j < particles.size(); j++)
	{
		rj = particles[j];
		r = glm::length(ri.position - rj.position);
		rSquared = r * r;

		if (0 <= r && h >= r) {
			density += M * (poly6constant * pow(hSquared - rSquared, 3));
		}
		else {
			density += 0.0f;
		}

	}


	return density;
}

glm::vec3 Fluid::calculatePressure(Particle ri)
{
	float gradPoly6constant = -(945.0f / (32 *pi * pow(h, 9)));

	Particle rj;
	glm::vec3 r;
	float rLength;
	float rLengthSquared;
	float hSquared = h * h;

	glm::vec3 pressureForce = glm::vec3(0.0f, 0.0f, 0.0f);
	for (size_t j = 0; j < particles.size(); j++)
	{
		rj = particles[j];
		r = ri.position - rj.position;
		rLength = glm::length(r);
		rLengthSquared = rLength * rLength;
		if (0 <= rLength && hSquared >= rLength) 
		{
			glm::vec3 kernel = (gradPoly6constant * r * pow(hSquared - rLengthSquared, 2));
			pressureForce += M * ((ri.localPressure + rj.localPressure) / (2.0f * rj.density)) * kernel;
		}
		else {
			pressureForce += glm::vec3(0.0f, 0.0f, 0.0f);
		}
	}


	return -pressureForce;
}

glm::vec3 Fluid::calculatePressureSpiky(Particle ri)
{
	float spikyConstant = -(45.0f / (pi * pow(h, 6)));

	Particle rj;
	glm::vec3 r;
	float rLength;
	float rLengthSquared;
	float hSquared = h * h;

	glm::vec3 pressureForce(0, 0, 0);

	for (int j = 0; j < particles.size(); j++) {
		rj = particles[j];
		r = ri.position - rj.position;
		rLength = glm::length(r);
		rLengthSquared = rLength * rLength;

		if (0 <= rLength && h >= rLength) {
			pressureForce += M * ((ri.localPressure + rj.localPressure) / (2 * rj.density)) *
				(spikyConstant * pow(h - rLength, 2) * r);
		}
		else {
			pressureForce += 0;
		}
	}

	return -pressureForce;
}

glm::vec3 Fluid::calculateViscosity(Particle ri)
{
	Particle rj;
	float vConstant = 45.0f / (pi * pow(h, 6));
	glm::vec3 r;
	float rLength;

	glm::vec3 viscosityForce(0, 0, 0);
	for (size_t j = 0; j < particles.size(); j++)
	{
		rj = particles[j];
		r = ri.position - rj.position;
		rLength = glm::length(r);

		if (0 <= rLength && h >= rLength)
		{
			viscosityForce += M * ((rj.velocity - ri.velocity) / rj.density) * (vConstant * (h - rLength));
		}
		else {
			viscosityForce += glm::vec3(0.0f, 0.0f, 0.0f);
		}
	}

	viscosityForce = u * viscosityForce;

	return viscosityForce;
}

void Fluid::calculateCircleVertices()
{
	circleVertices.push_back(glm::vec3(0,0,0));
	// generate the circle ( uses a triangle fan )
	for (double angle = 0; angle < 2 * pi; angle += 0.01) {
		glm::vec3 circlePoint(
			particleRadius * cos(angle),
			particleRadius * sin(angle),
			0
		);
		circleVertices.push_back(circlePoint);
	}

}
