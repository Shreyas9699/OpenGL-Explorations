#pragma once

#include "Renderer.h"

class Texture
{
private:
	unsigned int m_RendererID = 0;
	const char* m_FilePath;
	int m_W, m_H, m_BPP;

public:
	Texture(const char* filepath);
	~Texture();

	Texture(const Texture&) = delete;
	Texture& operator=(const Texture&) = delete;

	Texture(Texture&& o) noexcept;
	Texture& operator=(Texture&& o) noexcept;

	void Bind(unsigned int slot = 0) const;
	void Unbind() const;

	inline int GetWidth() const { return m_W; }
	inline int GetHeight() const { return m_H; }
};