#include <algorithm>

#include "Graphics.h"

///
// Shader member functions
///

Shader::Shader(unsigned int shaderType, const char* source)
{
	id = glCreateShader(shaderType);
	glShaderSource(id, 1, &source, NULL);
	successfulCompile = Compile();
}

Shader::Shader(unsigned int shaderType, std::string& source)
{
	const GLchar* c_source = source.c_str();
	glShaderSource(id, 1, &c_source, NULL);
	successfulCompile = Compile();
}

Shader::~Shader() 
{ 
	glDeleteShader(id); 
}

bool Shader::wasCompiled() 
{ 
	return successfulCompile; 
}

GLuint Shader::GetShaderID() { 
	return id; 
}

bool Shader::Compile()
{
	glCompileShader(id);

	//Check compile success
	GLint success = 0;
	glGetShaderiv(id, GL_COMPILE_STATUS, &success);
	if (success == GL_FALSE)
	{
		GLint logSize = 0;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &logSize);
		if (logSize > 0)
		{
			std::vector<GLchar> log(logSize);
			glGetShaderInfoLog(id, logSize, &logSize, &log[0]);
			fprintf(stderr, "Could not compile vertex shader \n%s\n", log);
			glDeleteShader(id);
		}
	}
	return success;
}

///
// Attribute
///

ShaderProgram::Attribute::Attribute(ShaderProgram* program, const char* name, int numArgs, GLenum argType, size_t sizeofType, bool isNormalized, int argStride, int argStart)
	: size{ numArgs }, type{ argType }, normalized{ (unsigned char)isNormalized }, stride{ argStride * (GLsizei)sizeofType }, start{ argStart * (GLsizei)sizeofType }
{
	index = glGetAttribLocation(program->id, name);
}

void ShaderProgram::Attribute::Apply()
{
	glEnableVertexAttribArray(index);
	glVertexAttribPointer(index, size, type, normalized, stride, (void*)(start));
}

///
// Shader Program member functions
///

ShaderProgram::ShaderProgram(Shader& vertexShader, Shader& fragmentShader)
{
	id = glCreateProgram();
	glAttachShader(id, vertexShader.GetShaderID());
	glAttachShader(id, fragmentShader.GetShaderID());
	glLinkProgram(id);
}
GLuint ShaderProgram::GetProgramID() 
{ 
	return id; 
}

void ShaderProgram::Use() 
{ 
	glUseProgram(id); 
}

void ShaderProgram::AddAttribute(const char* name, int numArgs, GLenum argType, size_t sizeofType, bool isNormalized, int argStride, int argStart)
{
	attributes.push_back(Attribute(this, name, numArgs, argType, sizeofType, isNormalized, argStride, argStart));
}

void ShaderProgram::ApplyAttributes()
{
	for each (Attribute attrib in attributes)
	{
		attrib.Apply();
	}
}