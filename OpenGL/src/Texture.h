#pragma once

#include "Renderer.h"

class Texture
{
private:
	unsigned int m_RendererID;
	const char* m_FilePath;
	unsigned char* m_LocalBuffer;
	int m_W, m_H, m_BPP;

public:
	Texture(const char* filepath);
	~Texture();

	void Bind(unsigned int slot = 0) const;
	void Unbind() const;

	inline int GetWidth() const { return m_W; }
	inline int GetHeight() const { return m_H; }
};