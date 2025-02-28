#pragma once

#include <GL/glew.h>

#include "IndexBuffer.h"
#include "VertexArray.h"
#include "Shader_t.h"

#define ASSERT(x) if(!(x)) __debugbreak();
#define GLCall(x) GLClearError();\
	x;\
	ASSERT(GLLogCall(#x, __FILE__, __LINE__))

// to check error in gl call, to get error, enclode the gl function with GLCall()
// example: glGenBuffers(1, &buffer) -> to get error GLCall(glGenBuffers(1, &buffer))
void GLClearError();
bool GLLogCall(const char* function, const char* file, int line);

class Renderer
{
public:
	void Clear() const;
	void Draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) const;
	void Draw(const VertexArray& va, const Shader& shader, float vertexCount) const;
	void DrawLines(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) const;
	void DrawLines(const VertexArray& va, const Shader& shader, float vertexCount) const;
	void DrawPatchesTri(const VertexArray& va, const Shader& shader, float vertexCount) const;
};