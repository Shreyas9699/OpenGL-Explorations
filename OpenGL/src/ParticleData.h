// ParticleData.h
#pragma once
#include "AttributeBuffer.h"
#include <string>
#include <unordered_map>
#include <memory>

template<typename T>
class ParticleData
{
public:
	std::unordered_map<std::string, std::unique_ptr<AttributeBuffer<T>>> m_Attributes;

	void AddAttribute(const std::string& name, size_t size) 
	{
		m_Attributes[name] = std::make_unique<AttributeBuffer<T>>(size);
	}

	AttributeBuffer<T>* GetAttribute(const std::string& name)
	{
		auto it = m_Attributes.find(name);
		if (it != m_Attributes.end())
		{
			return it->second.get();
		}
		return nullptr;
	}

	void UpdateBuffers()
	{
		for (auto& [key, buffers] : m_Attributes)
		{
			buffers->UpdateBuffer();
		}
	}

	void BindBuffers()
	{
		for (auto& [key, buffers] : m_Attributes)
		{
			buffers->BindBase(0);
		}
	}
};