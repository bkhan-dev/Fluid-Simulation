#define GLEW_STATIC
#include <GL/glew.h> 
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "GUI/imgui.h"
#include "GUI/imgui_impl_glfw.h"
#include "GUI/imgui_impl_opengl3.h"

#include "Shader.h"
#include "Camera.h"
#include "Tank.h"
#include "Fluid.h"

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

const int SCR_WIDTH = 800;
const int SCR_HEIGHT = 600;

Camera camera(glm::vec3(0.0f, 5.0f, 15.0f));

bool showGUI = true;
bool showParamWindow = true;
bool useDeltaTime = false;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

// if simulation should play or not
bool playAnimation = false;

// timestep
float time_step = 0.001;

int main(void)
{
	GLFWwindow* window;

	/* Initialize the library */
	if (!glfwInit())
		return -1;

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Fluid Simulation", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);
	//glfwSwapInterval(1);
	glfwSetKeyCallback(window, key_callback);
	glfwSetScrollCallback(window, scroll_callback);

	if (glewInit()) {
		std::cout << "failed glew" << std::endl;
		return 0;
	}

	//IMGUI SETUP
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();

	// Setup Platform/Renderer bindings
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	const char* glsl_version = "#version 330";
	ImGui_ImplOpenGL3_Init(glsl_version);

	// create shaders
	Shader _tankShader("tankVert.glsl", "tankFrag.glsl");

	// create water tank
	Tank tank;

	// create water blob
	Fluid fluid;

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		// calculate delta time
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		/* Render here */
		glClearColor(0.42f, 0.48f, 0.55f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		//IMGUI SETUP
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		// Setup GUI Windows
		if (showGUI)
		{
			ImGui::Begin("Simulation Setup");

			if (ImGui::Button("Play / Pause")) {
				playAnimation = !playAnimation;
			}

			if (ImGui::Button("Reset Sim")) {
				playAnimation = false;
				fluid.reset();
			}

			ImGui::Checkbox("Display Params", &showParamWindow);
			ImGui::Checkbox("Render Points", &fluid.renderPointParticles);

			ImGui::Separator();
			if (ImGui::Checkbox("Poly6 Pressure", &fluid.poly6KernelOn))
			{
				fluid.spikyKernelOn = false;
			}
			if (ImGui::Checkbox("Spiky Pressure", &fluid.spikyKernelOn)) {
				fluid.poly6KernelOn = false;
			}
			ImGui::Checkbox("Viscosity", &fluid.viscosityOn);

			ImGui::Separator();


			ImGui::End();
		}

		if (showParamWindow && showGUI) {
			ImGui::Begin("Parameters");

			ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.4f);
			//ImGui::InputFloat("Mass", &fluid.M, 0, 0, 5);
			ImGui::InputFloat("h - support radius", &fluid.h, 0, 0, 5);
			ImGui::InputFloat("p0 - rest density", &fluid.p0, 0, 0, 5);
			ImGui::InputFloat("k - gas constant", &fluid.k,0,0,5);
			ImGui::InputFloat("u - viscosity", &fluid.u, 0, 5, 5);
			ImGui::InputFloat("impulse force", &fluid.iForce, 0, 0, 3);
			ImGui::InputFloat("time step", &time_step, 0, 0, 5);
			ImGui::Checkbox("Use Delta Time", &useDeltaTime);

			ImGui::End();
		}

		// render the tank
		_tankShader.use();
		_tankShader.setVec4("colour", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		_tankShader.setMat4("projection", projection);
		glm::mat4 view = camera.GetViewMatrix();
		_tankShader.setMat4("view", view);
		glm::mat4 model = glm::mat4(1.0f);
		_tankShader.setMat4("model", model);
		tank.render();

		if (useDeltaTime) {
			time_step = deltaTime;
		}


		_tankShader.setVec4("colour", glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));
		if (playAnimation) {
			fluid.update(time_step);
		}

		fluid.render(_tankShader);

		// Render ImGui
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_Q && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		showGUI = !showGUI;
	}
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.Position.z -= yoffset;
}