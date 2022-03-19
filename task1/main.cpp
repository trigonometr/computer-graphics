#include <stdio.h>
#include <stdlib.h>

#include <GL/glew.h>

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#include <common/shader.hpp>

GLuint CreateVBO(const GLfloat *g_vertex_buffer_data, uint64_t size_bytes)
{
	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, size_bytes, g_vertex_buffer_data, GL_STATIC_DRAW);

	return vertexbuffer;
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

	GLFWwindow *window;
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

	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

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

	glm::mat4 View = glm::lookAt(
			glm::vec3(-2, 0, 2.5),
			glm::vec3(0, 0, 0),
			glm::vec3(0, 1, 0));
	glm::mat4 Model = glm::mat4(1.0f);
	glm::mat4 MVP = Projection * View * Model;

	glDisable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	do
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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
	glDeleteVertexArrays(1, &VertexArrayID);
	glDeleteProgram(program_red);
	glDeleteProgram(program_green);

	glfwTerminate();

	return 0;
}
