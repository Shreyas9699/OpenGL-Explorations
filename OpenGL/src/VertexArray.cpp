#include "VertexArray.h"
#include "VertexBufferLayout.h"
#include "Renderer.h"

VertexArray::VertexArray()
{
	GLCall(glGenVertexArrays(1, &m_RendererID));
}

VertexArray::~VertexArray()
{
    if (m_RendererID)
    {
        GLCall(glDeleteVertexArrays(1, &m_RendererID));
    }
}

VertexArray::VertexArray(VertexArray&& o) noexcept
    : m_RendererID(o.m_RendererID)
{
    o.m_RendererID = 0;
}

VertexArray& VertexArray::operator=(VertexArray&& o) noexcept
{
    if (this != &o)
    {
        GLCall(glDeleteVertexArrays(1, &m_RendererID));
        m_RendererID = o.m_RendererID;
        o.m_RendererID = 0;
    }
    return *this;
}

void VertexArray::AddBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout) 
{
    Bind();
    vb.Bind();
    const auto& elements = layout.GetElements();
    unsigned int offset = 0;
    for (unsigned int i = 0; i < elements.size(); i++) 
    {
        const auto& element = elements[i];
        glEnableVertexAttribArray(i);
        if (element.type == GL_INT || element.type == GL_UNSIGNED_INT) 
        {
            glVertexAttribIPointer(  i, element.count, element.type, layout.GetStride(), (const void*)(uintptr_t)offset);
        }
        else 
        {
            glVertexAttribPointer(i, element.count, element.type, element.normalized, layout.GetStride(), (const void*)(uintptr_t)offset);
        }
        offset += element.count * VertexBufferElement::GetSizeOfType(element.type);
    }
}

void VertexArray::Bind() const
{
	GLCall(glBindVertexArray(m_RendererID));
}

void VertexArray::Unbind() const
{
	GLCall(glBindVertexArray(0));
}
