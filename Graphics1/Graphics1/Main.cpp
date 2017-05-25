#include <iostream>
#include <string>
#include <fstream>
#include <vector>

#include "GL\glew.h"
#include "GLFW\glfw3.h"
#include "SOIL\SOIL.h"

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

	GLFWwindow *window = glfwCreateWindow(640, 480, "<3", NULL, NULL);

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

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f); //Set background color
	do
	{
		glClear(GL_COLOR_BUFFER_BIT); // Clear screen
		glEnableVertexAttribArray(0);

		mesh2.Draw();
		mesh.Draw();

		glDisableVertexAttribArray(0);

		glfwSwapBuffers(window);
		glfwPollEvents();
	} while (glfwWindowShouldClose(window) == false);

	delete shaderProgram;

	return 0;
}
