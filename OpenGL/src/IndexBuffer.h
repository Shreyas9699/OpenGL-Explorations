#pragma once

class IndexBuffer
{
private:
	unsigned int m_RendererID = 0;
	unsigned int m_Count;
public:
	// Creates IndexBuffer by Taking indices data and count of indices
	IndexBuffer(const unsigned int* data, unsigned int count);
	~IndexBuffer();

	IndexBuffer(const IndexBuffer&)			  = delete;
	IndexBuffer& operator=(const IndexBuffer&) = delete;

	IndexBuffer(IndexBuffer&& o) noexcept;
	IndexBuffer& operator=(IndexBuffer&& o) noexcept;

	void Bind() const;
	void Unbind() const;

	inline unsigned int GetCount() const { return m_Count; }
};