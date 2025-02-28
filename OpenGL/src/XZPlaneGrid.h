#pragma once
#include <GL/glew.h>

#include "Camera.h"
#include "Shader_t.h"

class XZPlaneGrid
{
private:
	std::unique_ptr<Shader> m_ShaderGrid;
	size_t m_GridSize;
	float m_AspectRatio;
	float m_Near, m_Far;
	unsigned int m_VAO;
public:
	XZPlaneGrid(float aspectRatio, size_t gridSize = -1, float near = 0.01f, float far = 1000.0f);
	~XZPlaneGrid();

	void UpdateAspectRation(float aspectRatio);
	void UpdateGridSize(float gridSize);
	void UpdateNear(float near);
	void UpdateFar(float far);
	void Render(const glm::mat4& view, const glm::mat4& projection);
};