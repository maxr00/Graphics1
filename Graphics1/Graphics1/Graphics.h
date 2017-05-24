#pragma once
#include <iostream>
#include <vector>
#include <string>

#include "GL\glew.h"
#include "GLFW\glfw3.h"

class Shader
{
	public:

		Shader(unsigned int shaderType, const char* source);

		Shader(unsigned int shaderType, std::string& source);

		~Shader();
		bool wasCompiled();

		GLuint GetShaderID();
	private:
		bool Compile();

		GLuint id = -1;
		bool successfulCompile = false;
};

class ShaderProgram
{
	friend class Mesh;

	public:
		ShaderProgram(Shader& vertexShader, Shader& fragmentShader);
		GLuint GetProgramID();
		void Use();
		void AddAttribute(const char* name, int numArgs, GLenum argType, size_t sizeofType, bool isNormalized, int argStride, int argStart);

	private:
		class Attribute
		{
			public:
				Attribute(ShaderProgram* program, const char* name, int numArgs, GLenum argType, size_t sizeofType, bool isNormalized, int argStride, int argStart);
				void Apply();

			private:
				GLuint index;
				GLint size;
				GLenum type;
				GLboolean normalized;
				GLsizei stride;
				GLsizei start;
		};

		void ApplyAttributes();

		GLuint id;
		std::vector<Attribute> attributes;
};

class Texture 
{
	int id;
};

class Mesh
{
	public:
		struct Vertice 
		{ 
			Vertice(float x, float y, float z, float r, float g, float b, float a) 
				: data{x,y,z,r,g,b,a} {}
			
			const static int vertDataVars = 7; // Number of variables a vertice holds
			float data[vertDataVars];
		};

		Mesh(ShaderProgram shaderProgram, GLenum renderMode = GL_TRIANGLES);

		void AddVertex(float x, float y, float z, float r, float g, float b, float a);

		void AddTriangle(
			float x1, float y1, float z1, float r1, float g1, float b1, float a1,
			float x2, float y2, float z2, float r2, float g2, float b2, float a2,
			float x3, float y3, float z3, float r3, float g3, float b3, float a3);

		void SetTexture(Texture tex) { texture = tex; }

		std::vector<Vertice>* GetVertices();
		void CompileMesh(); // Creates mesh with provided vertices

		void Draw();

	private:
		GLuint vaoID, vboID;
		GLenum drawMode = GL_TRIANGLES;
		std::vector<Vertice> vertices;
		Texture texture;
		ShaderProgram program;
};
