#include "IndexBuffer.h"
#include "Renderer.h"

IndexBuffer::IndexBuffer(const unsigned int* data, unsigned int count)
    : m_Count(count)
{
    ASSERT(sizeof(unsigned int) == sizeof(GLuint));

    GLCall(glGenBuffers(1, &m_RendererID));
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID));
    GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(unsigned int), data, GL_STATIC_DRAW));
}

IndexBuffer::~IndexBuffer()
{
    if (m_RendererID)
    {
        GLCall(glDeleteBuffers(1, &m_RendererID));
    }
}

IndexBuffer::IndexBuffer(IndexBuffer&& o) noexcept
    : m_RendererID(o.m_RendererID), m_Count(o.m_Count)
{
    o.m_RendererID = 0;
}

IndexBuffer& IndexBuffer::operator=(IndexBuffer&& o) noexcept
{
    if (this != &o)
    {
        ASSERT(sizeof(unsigned int) == sizeof(GLuint));
        GLCall(glDeleteBuffers(1, &m_RendererID));
        m_RendererID = o.m_RendererID;
        m_Count = o.m_Count;
        o.m_RendererID = 0;
    }
    return *this;
}

void IndexBuffer::Bind() const
{
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID));
}

void IndexBuffer::Unbind() const
{
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}
