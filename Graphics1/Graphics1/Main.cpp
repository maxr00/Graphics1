#include <iostream>
#include <string>
#include <fstream>
#include <vector>

#include "GL\glew.h"
#include "GLFW\glfw3.h"
#include "SOIL\SOIL.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Graphics.h"

std::string LoadFileToString(const char* filepath)
{
	std::string fileData = "";
	std::ifstream stream(filepath, std::ios::in);

	if (stream.is_open())
	{
		std::string line = "";
		while (getline(stream, line))
		{
			fileData += "\n" + line;
		}

		stream.close();
	}

	return fileData;
}

ShaderProgram* LoadShaders(const char* vertShaderPath, const char* fragShaderPath)
{
	std::string vertShaderSource = LoadFileToString(vertShaderPath).c_str();
	std::string fragShaderSource = LoadFileToString(fragShaderPath).c_str();

	return new ShaderProgram(Shader(GL_VERTEX_SHADER, vertShaderSource.c_str()), Shader(GL_FRAGMENT_SHADER, fragShaderSource.c_str()));
}

int main()
{

	if (glfwInit() == false)
	{
		glfwTerminate();
		fprintf(stderr, "Could not init GLFW");
		return 1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4); //4 AA
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow *window = glfwCreateWindow(800, 600, "<3", NULL, NULL);

	if (!window)
	{
		glfwTerminate();
		fprintf(stderr, "Could not create window");
		int i;
		std::cin >> i;
		return 1;
	}

	glfwMakeContextCurrent(window);

	if (glewInit() != GLEW_OK)
	{
		glfwTerminate();
		fprintf(stderr, "Could not init GLEW");
		return 1;
	}


	// Load Shader Program
	ShaderProgram* shaderProgram = LoadShaders("shader.vertshader", "shader.fragshader");

	if (!shaderProgram->wasCompiled())
	{
		glfwTerminate();
		fprintf(stderr, "Could not compile shader");
		int a;
		std::cin >> a;
		return 1;
	}

	shaderProgram->AddAttribute("pos", 3, GL_FLOAT, sizeof(GL_FLOAT), false, 9, 0);
	shaderProgram->AddAttribute("color", 4, GL_FLOAT, sizeof(GL_FLOAT), false, 9, 3);
	shaderProgram->AddAttribute("texcoord", 2, GL_FLOAT, sizeof(GL_FLOAT), false, 9, 7);


	GLuint program = shaderProgram->GetProgramID();

	// Create triangle
	Mesh mesh = Mesh(*shaderProgram);
	mesh.AddTriangle(
		//  x,     y, z,    r,    g,    b, a, s, t
		-0.5f, -0.5f, 0, 1.0f, 0.0f, 0.0f, 1, 0, 0,
		 0.5f, -0.5f, 0, 0.0f, 1.0f, 0.0f, 1, 1, 0,
		-0.5f,  0.5f, 0, 0.0f, 0.0f, 1.0f, 1, 0, 1
	);
	mesh.AddTriangle(
		//  x,     y, z,    r,    g,    b, a, s, t
		-0.5f,  0.5f, 0, 0.0f, 0.0f, 1.0f, 1, 0, 1,
		 0.5f,  0.5f, 0, 1.0f, 0.0f, 0.0f, 1, 1, 1,
		 0.5f, -0.5f, 0, 0.0f, 1.0f, 0.0f, 1, 1, 0
	);
	mesh.CompileMesh();
	Texture tex = Texture("sample.png");
	mesh.SetTexture(&tex);


	Mesh mesh2 = Mesh(*shaderProgram);
	mesh2.AddTriangle(
		//  x,     y, z,    r,    g,    b, a, s, t
		-1.0f, -1.0f, 0, 1.0f, 0.0f, 0.0f, 1, 0, 0,
		1.0f, -1.0f, 0, 0.0f, 1.0f, 0.0f, 1, 1, 0,
		-1.0f, 1.0f, 0, 0.0f, 0.0f, 1.0f, 1, 0, 1
	);
	mesh2.AddTriangle(
		//  x,     y, z,    r,    g,    b, a, s, t
		-1.0f, 1.0f, 0, 0.0f, 0.0f, 1.0f, 1, 0, 1,
		1.0f, 1.0f, 0, 1.0f, 0.0f, 0.0f, 1, 1, 1,
		1.0f, -1.0f, 0, 0.0f, 1.0f, 0.0f, 1, 1, 0
	);
	mesh2.CompileMesh();
	mesh2.SetTexture(NULL);

	shaderProgram->Use();


	//

	//Model (Transform)
	glm::mat4 trans;
	//trans = glm::scale(trans, glm::vec3(0.5f, 0.5f, 1.0f));
	//trans = glm::translate(trans, glm::vec3(0, 0, 0));
	//trans = glm::rotate(trans, glm::radians(0.0f), glm::vec3(0, 0, 1)); // transform matrix

	GLint uniModel = glGetUniformLocation(shaderProgram->GetProgramID(), "model");
	glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(trans)); // Give "trans" attrib the trans matrix

	//View
	glm::mat4 view = glm::lookAt(
		glm::vec3(1.2f, 1.2f, 1.2f), // Camera position
		glm::vec3(0.0f, 0.0f, 0.0f), // Point centered on screen
		glm::vec3(0.0f, 0.0f, 1.0f)  // Up vector (1 on Y axis implies XZ is "ground")
	);
	GLint uniView = glGetUniformLocation(shaderProgram->GetProgramID(), "view");
	glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(view));

	//Projection
	glm::mat4 proj = glm::perspective(
		glm::radians(45.0f), // Field of view (vertical angle of camera view)
		800.0f/600.0f,			 // Screen aspect ratio
		1.0f,								 // Near clipping
		10.0f 							 // Far clipping
	);

	//proj = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f);
	GLint uniProj = glGetUniformLocation(shaderProgram->GetProgramID(), "proj");
	glUniformMatrix4fv(uniProj, 1, GL_FALSE, glm::value_ptr(proj));

	//

	glEnable(GL_DEPTH_TEST);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f); //Set background color
	do
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear screen
		glEnableVertexAttribArray(0);

		glm::mat4 trans;
		trans = glm::translate(trans, glm::vec3(-0.5f, -0.0f, -0.0f));
		trans = glm::rotate(trans, glm::radians(90.0f), glm::vec3(0, 1, 0)); // transform matrix
		trans = glm::rotate(trans, glm::radians(-45.0f), glm::vec3(1, 0, 0));
		trans = glm::scale(trans, glm::vec3(0.5f, 0.5f, 1.0f));
		glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(trans));
		mesh2.Draw();

		trans = glm::mat4();
		glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(trans));
		mesh.Draw();

		glDisableVertexAttribArray(0);

		glfwSwapBuffers(window);
		glfwPollEvents();
	} while (glfwWindowShouldClose(window) == false);

	delete shaderProgram;

	return 0;
}
