#pragma once

#include <glm/glm.hpp>
#include <vector>
#include "UtilityStructs.h"

class TerrainChunk
{
public:
	Object m_Plane;

	TerrainChunk(int x, int z, float chunkSize, int rez);
	void GenerateMesh();
	void Render();

	glm::mat4 GetModelMatrix() const;
	AABB GetAABB(float maxHeight) const;
	int GetResolution() const;
	std::pair<int, int> GetChunkCoordinates() const;

private:
	int m_X, m_Z;
	float m_ChunkSize;
	int m_Resolution;
	std::vector<GLfloat> m_Vertices;
	std::vector<GLuint> m_Indices;
};
