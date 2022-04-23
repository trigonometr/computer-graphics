#include <stdio.h>
#include <stdlib.h>

#include <GL/glew.h>

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#include <common/shader.hpp>

GLFWwindow *window;

GLuint CreateVBO(const GLfloat *g_vertex_buffer_data, uint64_t size_bytes)
{
	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, size_bytes, g_vertex_buffer_data, GL_STATIC_DRAW);

	return vertexbuffer;
}

glm::mat4 ViewMatrix;

glm::mat4 getViewMatrix()
{
	return ViewMatrix;
}

// Initial position : on +Z
glm::vec3 position = glm::vec3(0, 2, 5);

float angle = 3.14f / 2;
float speed = 0.5f;
float radius = 5.0f;
float speed_rad = 3.14f * speed / radius;

void computeMatricesFromInputs()
{

	// glfwGetTime is called only once, the first time this function is called
	static double lastTime = glfwGetTime();

	// Compute time difference between current and last frame
	double currentTime = glfwGetTime();
	float deltaTime = float(currentTime - lastTime);

	angle += deltaTime * speed_rad;
	position = glm::vec3(5 * cos(angle), 0, 5 * sin(angle));

	ViewMatrix = glm::lookAt(
			position,
			glm::vec3(0, 0, 0),
			glm::vec3(0, 1, 0));

	// For the next frame, the "last time" will be "now"
	lastTime = currentTime;
}

int main(void)
{
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		getchar();
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(1024, 768, "Playground", NULL, NULL);
	if (window == NULL)
	{
		fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
		getchar();
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	glewExperimental = true;
	if (glewInit() != GLEW_OK)
	{
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return -1;
	}

	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	glClearColor(0.4f, 0.0f, 0.2f, 0.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	GLuint program_red = LoadShaders("AwesomeVertexShader.vertexshader", "AwesomeFragmentShader.fragmentshader");
	GLuint program_green = LoadShaders("AmazingVertexShader.vertexshader", "AmazingFragmentShader.fragmentshader");

	static const GLfloat g_vertex_buffer_data_red[] = {
			-1.0f, -1.0f, 0.0f,
			1.0f, -1.0f, 0.0f,
			-1.0f, 1.0f, 0.0f};

	static const GLfloat g_vertex_buffer_data_green[] = {
			-1.0f, -1.0f, 1.0f,
			1.0f, -1.0f, 1.0f,
			-1.0f, 1.0f, -1.0f};

	GLuint vertexbuffer_1 = CreateVBO(g_vertex_buffer_data_red, sizeof(g_vertex_buffer_data_red));
	GLuint vertexbuffer_2 = CreateVBO(g_vertex_buffer_data_green, sizeof(g_vertex_buffer_data_green));

	GLuint MatrixID1 = glGetUniformLocation(program_red, "MVP");
	GLuint MatrixID2 = glGetUniformLocation(program_green, "MVP");

	glm::mat4 Projection = glm::perspective(glm::radians(60.0f), 4.0f / 3.0f, 0.1f, 100.0f);
	glm::mat4 Model = glm::mat4(1.0f);

	glDisable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	double lastTime = glfwGetTime();
	int frames = 0;

	do
	{
		double currentTime = glfwGetTime();
		++frames;

		if (currentTime - lastTime >= 1)
		{
			printf("%d FPS\n", frames);
			frames = 0;
			lastTime += 1.0;
		}

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		computeMatricesFromInputs();
		glm::mat4 View = getViewMatrix();

		glm::mat4 MVP = Projection * View * Model;

		glUseProgram(program_red);

		glUniformMatrix4fv(MatrixID1, 1, GL_FALSE, &MVP[0][0]);

		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer_1);
		glVertexAttribPointer(
				0,
				3,
				GL_FLOAT,
				GL_FALSE,
				0,
				(void *)0);
		glDrawArrays(GL_TRIANGLES, 0, 3);

		glUseProgram(program_green);

		glUniformMatrix4fv(MatrixID2, 1, GL_FALSE, &MVP[0][0]);

		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer_2);
		glVertexAttribPointer(
				1,
				3,
				GL_FLOAT,
				GL_FALSE,
				0,
				(void *)0);

		glDrawArrays(GL_TRIANGLES, 0, 3);

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);

		glfwSwapBuffers(window);
		glfwPollEvents();
	} while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
					 glfwWindowShouldClose(window) == 0);

	glDeleteBuffers(1, &vertexbuffer_1);
	glDeleteBuffers(1, &vertexbuffer_2);
	glDeleteProgram(program_red);
	glDeleteProgram(program_green);

	glfwTerminate();

	return 0;
}
