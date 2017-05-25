#pragma once
#include <iostream>
#include <vector>
#include <string>

#include "GL\glew.h"
#include "GLFW\glfw3.h"
#include "SOIL\SOIL.h"

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
		~ShaderProgram();

		bool wasCompiled();
		GLuint GetProgramID();
		void Use();
		void AddAttribute(const char* name, int numArgs, GLenum argType, size_t sizeofType, bool isNormalized, int argStride, int argStart);

		void ApplyAttributes();
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


		GLuint id;
		std::vector<Attribute> attributes;
		bool successfulCompile = false;
};

class Texture 
{
	public:
		Texture(const char* file)
		{
			int width, height;
			unsigned char* image = SOIL_load_image(file, &width, &height, 0, SOIL_LOAD_RGB);

			glGenTextures(1, &id);
			glBindTexture(GL_TEXTURE_2D, id);

			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image); // Load image
			SOIL_free_image_data(image); // Data given to opengl, dont need it here anymore

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

			glGenerateMipmap(GL_TEXTURE_2D);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		}

		Texture(float *pixels, int width, int height)
		{
			glGenTextures(1, &id);
			glBindTexture(GL_TEXTURE_2D, id);

			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_FLOAT, pixels); // Load image

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

			glGenerateMipmap(GL_TEXTURE_2D);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		}

		~Texture() 
		{ 
			glDeleteTextures(1, &id); 
		}

		GLuint GetID() { return id; }

	private:
		GLuint id;
};

class Mesh
{
	public:
		struct Vertice 
		{ 
			Vertice(float x, float y, float z, float r, float g, float b, float a, float texX, float texY) 
				: data{x,y,z,r,g,b,a,texX, 1 - texY} {} //Flip y because image is loaded upside down
			
			const static int vertDataVars = 3 + 4 + 2; // Number of variables a vertice holds
			float data[vertDataVars];
		};

		Mesh(ShaderProgram& shaderProgram, GLenum renderMode = GL_TRIANGLES);
		~Mesh();

		void AddVertex(float x, float y, float z, float r, float g, float b, float a, float texX, float texY);

		void AddTriangle(
			float x1, float y1, float z1, float r1, float g1, float b1, float a1, float texX1, float texY1,
			float x2, float y2, float z2, float r2, float g2, float b2, float a2, float texX2, float texY2,
			float x3, float y3, float z3, float r3, float g3, float b3, float a3, float texX3, float texY3);

		void SetTexture(Texture* tex) { texture = tex; }

		std::vector<Vertice>* GetVertices();
		void CompileMesh(); // Creates mesh with provided vertices

		void Draw();

	private:
		GLuint vaoID, vboID;
		GLenum drawMode = GL_TRIANGLES;
		std::vector<Vertice> vertices;
		Texture* texture;
		ShaderProgram program;
};
