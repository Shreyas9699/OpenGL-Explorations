#pragma once
#include <iostream>
#include <vector>
#include <glm/glm.hpp>

#include "VertexArray.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"

struct LightColor
{
	glm::vec3 baseColor{ 1.0f };

	glm::vec3 ambient{};
	glm::vec3 diffuse{};
	glm::vec3 specular{};

	float ambientStrength = 1.0f;
	float diffuseStrength = 1.0f;
	float specularStrength = 1.0f;

	void UpdateADS()
	{
		ambient = baseColor * ambientStrength;
		diffuse = baseColor * diffuseStrength;
		specular = baseColor * specularStrength;
	}
};

struct Light : LightColor
{
	glm::vec3 position;
};

struct DirectionalLight : LightColor
{
	glm::vec3 direction{};
};

struct PointLight : LightColor
{
	glm::vec3 position;

	float constant;
	float linear;
	float quadratic;
};

struct FlashLight
{
	glm::vec3 position;
	glm::vec3 direction;
	float cutOff;
	float outerCutOff;

	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;

	float constant;
	float linear;
	float quadratic;
};

struct AABB
{
	glm::vec3 min;
	glm::vec3 max;

	glm::vec3 GetCenter() const 
	{
		return (min + max) * 0.5f;
	}

	float GetRadiusAlongDirection(const glm::vec3& dir) const 
	{
		const glm::vec3 extents = (max - min) * 0.5f;
		return extents.x * std::abs(dir.x) +
			extents.y * std::abs(dir.y) +
			extents.z * std::abs(dir.z);
	}
};

struct Object
{
	std::unique_ptr<VertexArray> VAO;
	std::unique_ptr<VertexBuffer> VBO;
	std::unique_ptr<IndexBuffer> IBO;
};