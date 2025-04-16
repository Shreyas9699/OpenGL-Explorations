// AttributeBuffer.h
#pragma once
#include <GL/glew.h>
#include <vector>

template<typename T>
class AttributeBuffer
{
public:
	std::vector<T> data;
	GLuint ssboID = 0;

	// Constructor: allocate and initialize with a given count
	AttributeBuffer(size_t size)
	{
		data.resize(size);
		glGenBuffers(1, &ssboID);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboID);
		glBufferData(GL_SHADER_STORAGE_BUFFER, size * sizeof(T), data.data(), GL_DYNAMIC_DRAW);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	}

	~AttributeBuffer()
	{
		data.clear();
		glDeleteBuffers(1, &ssboID);
	}

	// Resize the container and reallocate the buffer.
	void Resize(size_t size)
	{
		data.resize(size);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboID);
		glBufferData(GL_SHADER_STORAGE_BUFFER, size * sizeof(T), data.data(), GL_DYNAMIC_DRAW);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	}

	// Update SSBO with the data on CPU.
	void UpdateBuffer()
	{
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboID);
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, data.size() * sizeof(T), data.data());
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	}

	// Bind the SSBO to a binding point.
	void BindBase(GLuint bindingPoint)
	{
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, bindingPoint, ssboID);
	}
};