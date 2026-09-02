#include "Texture.h"
#include "stb_image/stb_image.h"

Texture::Texture(const char* filepath)
	:m_RendererID(0), m_FilePath(filepath), m_W(0), m_H(0), m_BPP(0)
{
	stbi_set_flip_vertically_on_load(1);
	unsigned char* localBuffer = stbi_load(filepath, &m_W, &m_H, &m_BPP, 4);

	if (localBuffer)
	{
		GLCall(glGenTextures(1, &m_RendererID));
		GLCall(glActiveTexture(GL_TEXTURE0));
		GLCall(glBindTexture(GL_TEXTURE_2D, m_RendererID));

		// Set texture parameters
		GLCall(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));
		GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
		GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
		GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
		GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

		// Upload texture data
		GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_W, m_H, 0, GL_RGBA, GL_UNSIGNED_BYTE, localBuffer));
		GLCall(glBindTexture(GL_TEXTURE_2D, 0));

		// Debug output
		std::cout << "Texture loaded: " << m_W << "x" << m_H << " (" << m_BPP << " BPP)" << std::endl;
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(localBuffer);
}

Texture::Texture(unsigned int existingId, int w, int h)
	: m_RendererID(existingId), m_FilePath(nullptr), m_W(w), m_H(h), m_BPP(0)
{}

Texture::~Texture()
{
	if (m_RendererID)
	{
		GLCall(glDeleteTextures(1, &m_RendererID));
	}
}

Texture::Texture(Texture&& o) noexcept
	: m_RendererID(o.m_RendererID), 
	m_FilePath(o.m_FilePath), 
	m_W(o.m_W), 
	m_H(o.m_H), 
	m_BPP(o.m_BPP)
{
	o.m_RendererID = 0;
}

Texture& Texture::operator=(Texture&& o) noexcept
{
	if (this != &o)
	{
		GLCall(glDeleteTextures(1, &m_RendererID));
		m_RendererID = o.m_RendererID;
		m_FilePath = o.m_FilePath;
		m_W = o.m_W;
		m_H = o.m_H;
		m_BPP = o.m_BPP;
		o.m_RendererID = 0;
	}
	return *this;
}

void Texture::Bind(unsigned int slot) const
{
	GLCall(glActiveTexture(GL_TEXTURE0 + slot));
	GLCall(glBindTexture(GL_TEXTURE_2D, m_RendererID));
}

void Texture::Unbind() const
{
	GLCall(glBindTexture(GL_TEXTURE_2D, 0));
}
