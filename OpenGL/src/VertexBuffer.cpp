#include "VertexBuffer.h"
#include "Renderer.h"

VertexBuffer::VertexBuffer(const void* data, unsigned int size, GLenum bufferType, GLenum usage)
    : m_BufferType(bufferType)
{
    GLCall(glGenBuffers(1, &m_RendererID));
    GLCall(glBindBuffer(m_BufferType, m_RendererID));
    GLCall(glBufferData(m_BufferType, size, data, usage));
    GLCall(glBindBuffer(m_BufferType, 0));
}

VertexBuffer::~VertexBuffer()
{
    GLCall(glDeleteBuffers(1, &m_RendererID));
}

void VertexBuffer::Bind() const
{
    GLCall(glBindBuffer(m_BufferType, m_RendererID));
}

void VertexBuffer::Unbind() const
{
    GLCall(glBindBuffer(m_BufferType, 0));
}

void VertexBuffer::UpdateData(const void* data, unsigned int size, unsigned int offset) const
{
    GLCall(glBindBuffer(m_BufferType, m_RendererID));
    GLCall(glBufferSubData(m_BufferType, offset, size, data));
}

void VertexBuffer::BindBase(GLenum target, unsigned int index) const
{
    GLCall(glBindBufferBase(target, index, m_RendererID));
}
