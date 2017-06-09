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

ShaderProgram* LoadShaders(const char* vertShaderPath, const char* fragShaderPath, std::vector<ShaderProgram::Attribute>& attribs)
{
	std::string vertShaderSource = LoadFileToString(vertShaderPath).c_str();
	std::string fragShaderSource = LoadFileToString(fragShaderPath).c_str();

	return new ShaderProgram(Shader(GL_VERTEX_SHADER, vertShaderSource.c_str()), Shader(GL_FRAGMENT_SHADER, fragShaderSource.c_str()), attribs);
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
	std::vector<ShaderProgram::Attribute> attribs;
	attribs.push_back(ShaderProgram::Attribute("pos", 3, GL_FLOAT, sizeof(GL_FLOAT), false, 9, 0));
	attribs.push_back(ShaderProgram::Attribute("color", 4, GL_FLOAT, sizeof(GL_FLOAT), false, 9, 3));
	attribs.push_back(ShaderProgram::Attribute("texcoord", 2, GL_FLOAT, sizeof(GL_FLOAT), false, 9, 7));

	ShaderProgram* shaderProgram = LoadShaders("shader.vertshader", "shader.fragshader", attribs);

	if (!shaderProgram->wasCompiled())
	{
		glfwTerminate();
		fprintf(stderr, "Could not compile shader");
		int a;
		std::cin >> a;
		return 1;
	}

	shaderProgram->Use();

	GLuint program = shaderProgram->GetProgramID();

	// Create triangle
	Mesh mesh = Mesh(*shaderProgram);
	mesh.AddTriangle(
		//  x,     y, z,    r,    g,    b, a, s, t
		-0.5f,  0.5f, 0, 0.0f, 0.0f, 1.0f, 1, 0, 1, // Top Left
		-0.5f, -0.5f, 0, 1.0f, 0.0f, 0.0f, 1, 0, 0, // Bot Left
		 0.5f, -0.5f, 0, 0.0f, 1.0f, 0.0f, 1, 1, 0  // Bot Right
	);
	mesh.AddTriangle(
		//  x,     y, z,    r,    g,    b, a, s, t
		 0.5f, -0.5f, 0, 0.0f, 1.0f, 0.0f, 1, 1, 0, // Bot Right
		 0.5f,  0.5f, 0, 1.0f, 0.0f, 0.0f, 1, 1, 1, // Top Right
		-0.5f,  0.5f, 0, 0.0f, 0.0f, 1.0f, 1, 0, 1  // Top Left
	);
	mesh.CompileMesh();
	Texture tex = Texture("sampleBlend.png");
	mesh.SetTexture(&tex);


	Mesh mesh2 = Mesh(*shaderProgram);
	mesh2.AddTriangle(
		//  x,     y, z,    r,    g,    b, a, s, t
		-1.0f, -1.0f, 0, 1.0f, 0.0f, 0.0f, 1, 0, 0, // Bot Left
		 1.0f, -1.0f, 0, 0.0f, 1.0f, 0.0f, 1, 1, 0, // Bot Right
		-1.0f,  1.0f, 0, 0.0f, 0.0f, 1.0f, 1, 0, 1  // Top Left
	);
	mesh2.AddTriangle(
		//  x,     y, z,    r,    g,    b, a, s, t
		 1.0f, -1.0f, 0, 0.0f, 1.0f, 0.0f, 1, 1, 0, // Bot Right
		 1.0f,  1.0f, 0, 1.0f, 0.0f, 0.0f, 1, 1, 1, // Top Right
		-1.0f,  1.0f, 0, 0.0f, 0.0f, 1.0f, 1, 0, 1  // Top Left
	);
	mesh2.CompileMesh();
	mesh2.SetTexture(NULL);


	mesh2.transform.SetPosition({ -0.5f, 0.0f, 0.0f });
	mesh2.transform.SetRotation({ -45.0f, 90, 0 });
	mesh2.transform.SetScale({ 0.5f, 0.5f, 1.0f });

	//Camera
	Camera main;
	main.SetView(glm::vec3(0, 0, 2.0f), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	main.SetProjection(45.0f, 800.0f / 600.0f, 1, 10);

	glEnable(GL_DEPTH_TEST);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK); //Dont render back

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f); //Set background color

	float dt = 0.0f, last = 0.0f;
	do //Main Loop
	{
		float currentFrame = glfwGetTime();
		dt = currentFrame - last;
		last = currentFrame;

		main.OrbitAround(glm::vec3(0, 0.2f, 2.2f), dt * 120, glm::vec3(1, 0, 0));
		
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear buffers
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
