#pragma once
#define GLEW_STATIC
#include <GL/glew.h> 
#include <vector>
class Tank
{
public:
	Tank();
	~Tank();

	const float width = 5.0f;
	const float height = 5.0f;

	std::vector<float> vertices = {
		-2.5f, 5.0f, 0.0f,
		-2.5f, 0.0f, 0.0f,

		-2.5f, 0.0f, 0.0f,
		2.5f, 0.0f, 0.0f,

		2.5f, 0.0f, 0.0f,
		2.5f, 5.0f, 0.0f
	};

	void render();
private:
	unsigned int VAO, VBO;
};

