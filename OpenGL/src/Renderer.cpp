#include "Renderer.h"

#include <iostream>

void GLClearError()
{
	while (glGetError() != GL_NO_ERROR);
}

bool GLLogCall(const char* function, const char* file, int line)
{
	while (GLenum error = glGetError())
	{
		std::cout << "[OpenGL Error] (" << error << "): " << function << " " << file << ": " << line << std::endl;
		return false;
	}
	return true;
}


void Renderer::Draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) const
{
	shader.Bind();
	va.Bind();
	ib.Bind();
	GLCall(glDrawElements(GL_TRIANGLES, ib.GetCount(), GL_UNSIGNED_INT, nullptr));
}

void Renderer::Draw(const VertexArray& va, const Shader& shader, float vertexCount) const
{
	shader.Bind();
	va.Bind();
	GLCall(glDrawArrays(GL_TRIANGLES, 0, vertexCount));
}

void Renderer::DrawLines(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) const
{
	shader.Bind();
	va.Bind();
	ib.Bind();
	GLCall(glDrawElements(GL_LINES, ib.GetCount(), GL_UNSIGNED_INT, nullptr););
}

void Renderer::DrawLines(const VertexArray& va, const Shader& shader, float vertexCount) const
{
	shader.Bind();
	va.Bind();
	GLCall(glDrawArrays(GL_LINE_STRIP, 0, vertexCount));
}

void Renderer::DrawPatchesTri(const VertexArray& va, const Shader& shader, float vertexCount) const
{
	shader.Bind();
	va.Bind();
	// Set OpenGL to interpret the input as patches (instead of triangles)
	glPatchParameteri(GL_PATCH_VERTICES, 3);
	GLCall(glDrawArrays(GL_PATCHES, 0, vertexCount / 3));
}

void Renderer::Clear() const
{
	GLCall(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
	GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
}