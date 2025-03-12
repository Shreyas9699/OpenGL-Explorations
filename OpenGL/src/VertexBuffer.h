#pragma once
#include <GL/glew.h>

class VertexBuffer
{
private:
	unsigned int m_RendererID;
	GLenum m_BufferType;
public:
	VertexBuffer(const void* data, unsigned int size, GLenum bufferType = GL_ARRAY_BUFFER, GLenum usage = GL_STATIC_DRAW);
	~VertexBuffer();

	void Bind() const;
	void Unbind() const;
	void UpdateData(const void* data, unsigned int size, unsigned int offset = 0) const;
	void BindBase(GLenum target, unsigned int index) const;

	inline unsigned int GetRendererID() const { return m_RendererID; }
};