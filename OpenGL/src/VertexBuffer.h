#pragma once
#include <GL/glew.h>

class VertexBuffer
{
private:
	unsigned int m_RendererID = 0;
	GLenum m_BufferType;
public:
	VertexBuffer(const void* data, unsigned int size, GLenum bufferType = GL_ARRAY_BUFFER, GLenum usage = GL_STATIC_DRAW);
	~VertexBuffer();

	VertexBuffer(const VertexBuffer&)		   = delete;
	VertexBuffer& operator=(const VertexBuffer&) = delete;

	VertexBuffer(VertexBuffer&& o) noexcept;
	VertexBuffer& operator=(VertexBuffer&& o) noexcept;

	void Bind() const;
	void Unbind() const;
	void UpdateData(const void* data, unsigned int size, unsigned int offset = 0) const;
	void BindBase(GLenum target, unsigned int index) const;
	unsigned int GetBufferID() const { return m_RendererID; }

	inline unsigned int GetRendererID() const { return m_RendererID; }
};