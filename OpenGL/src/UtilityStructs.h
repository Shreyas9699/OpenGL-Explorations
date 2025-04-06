#pragma once
#include <iostream>
#include <vector>
#include <glm/glm.hpp>

#include "VertexArray.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"


struct Light
{
	glm::vec3 position;

	glm::vec3 baseColor;
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;

	float ambientStrength;
	float diffuseStrength;
	float specularStrength;
};

struct DirectionalLight
{
	glm::vec3 baseColor;
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;

	float ambientStrength;
	float diffuseStrength;
	float specularStrength;
};

struct PointLight
{
	glm::vec3 position;

	glm::vec3 baseColor;
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;

	float ambientStrength;
	float diffuseStrength;
	float specularStrength;

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