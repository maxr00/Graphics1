#include "Graphics.h"

///
// Mesh
///
Mesh::Mesh(ShaderProgram shaderProgram, GLenum renderMode) 
	: program{ shaderProgram }, drawMode{renderMode}
{
	glGenVertexArrays(1, &vaoID);
	glBindVertexArray(vaoID);

	glGenBuffers(1, &vboID); //Start vbo
	glBindBuffer(GL_ARRAY_BUFFER, vboID);

	program.ApplyAttributes(); // Apply program attributes for vao to remember
}

void Mesh::AddVertex(float x, float y, float z, float r, float g, float b, float a)
{
	Vertice vert = Vertice{ x, y, z, r, g, b, a };
	vertices.push_back(vert);
}

void Mesh::AddTriangle(
	float x1, float y1, float z1, float r1, float g1, float b1, float a1,
	float x2, float y2, float z2, float r2, float g2, float b2, float a2,
	float x3, float y3, float z3, float r3, float g3, float b3, float a3)
{
	AddVertex(x1, y1, z1, r1, g1, b1, a1);
	AddVertex(x2, y2, z2, r2, g2, b2, a2);
	AddVertex(x3, y3, z3, r3, g3, b3, a3);
}

void Mesh::Draw()
{
	glBindVertexArray(vaoID);
	glBindBuffer(GL_ARRAY_BUFFER, vboID);
	glDrawArrays(drawMode, 0, (int)vertices.size());
}

std::vector<Mesh::Vertice>* Mesh::GetVertices()
{
	return &vertices;
}

void Mesh::CompileMesh()
{
	glBindVertexArray(vaoID); // Enable vao to ensure correct mesh is compiled
	glBindBuffer(GL_ARRAY_BUFFER, vboID); // Enable vbo
	
	int s = vertices.size() * sizeof(float) * Vertice::vertDataVars;
	float* verts = new float[s];
	
	int i = 0;
	for each(auto vert in vertices)
		for (int j = 0; j < sizeof(vert) / sizeof(float); ++j, ++i)
			verts[i] = vert.data[j];

	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * s, verts, GL_STATIC_DRAW); //Give vbo verts
	delete [] verts;
}
