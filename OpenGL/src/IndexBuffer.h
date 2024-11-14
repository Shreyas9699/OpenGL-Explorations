#pragma once

class IndexBuffer
{
private:
	unsigned int m_RendereID;
	unsigned int m_Count;
public:
	// Creates IndexBuffer by Taking indices data and count of indices
	IndexBuffer(const unsigned int* data, unsigned int count);
	~IndexBuffer();

	void Bind() const;
	void Unbind() const;

	inline unsigned int GetCount() const { return m_Count; }
};