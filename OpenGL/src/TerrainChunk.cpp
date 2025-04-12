#include "TerrainChunk.h"
#include "VertexBufferLayout.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include <glm/gtx/transform.hpp>
#include <cmath>

TerrainChunk::TerrainChunk(int x, int z, float chunkSize, int rez)
    : m_X(x), m_Z(z), m_ChunkSize(chunkSize), m_Resolution(rez)
{
    GenerateMesh();
}

void TerrainChunk::GenerateMesh()
{
    m_Vertices.clear();
    m_Indices.clear();

    m_Plane.VAO.reset();
    m_Plane.VBO.reset();
    m_Plane.IBO.reset();

    float gridSpacingX = m_ChunkSize / (m_Resolution - 1);
    float gridSpacingZ = m_ChunkSize / (m_Resolution - 1);

    // Generate vertices
    m_Vertices.reserve(m_Resolution * m_Resolution * 3);

    // Add position and edge flag data
    for (int z = 0; z < m_Resolution; z++)
    {
        for (int x = 0; x < m_Resolution; x++)
        {
            float xPos = (x * gridSpacingX) + (m_X * m_ChunkSize);
            float zPos = (z * gridSpacingZ) + (m_Z * m_ChunkSize);

            // Position
            m_Vertices.push_back(xPos);
            m_Vertices.push_back(0.0f);
            m_Vertices.push_back(zPos);
        }
    }

    // Generate indices for patches (quads)
    m_Indices.reserve((m_Resolution - 1) * (m_Resolution - 1) * 4);
    for (int z = 0; z < m_Resolution - 1; z++)
    {
        for (int x = 0; x < m_Resolution - 1; x++)
        {
            int topLeft = z * m_Resolution + x;
            int topRight = topLeft + 1;
            int bottomLeft = (z + 1) * m_Resolution + x;
            int bottomRight = bottomLeft + 1;

            // Quad indices: top-left, top-right, bottom-right, bottom-left
            m_Indices.push_back(topLeft);
            m_Indices.push_back(topRight);
            m_Indices.push_back(bottomRight);
            m_Indices.push_back(bottomLeft);
        }
    }

    // Set up OpenGL buffers
    m_Plane.VAO = std::make_unique<VertexArray>();
    m_Plane.VBO = std::make_unique<VertexBuffer>(m_Vertices.data(), static_cast<unsigned int>(m_Vertices.size() * sizeof(float)));

    VertexBufferLayout layout;
    layout.Push<float>(3);    // position (xyz)
    m_Plane.VAO->AddBuffer(*m_Plane.VBO, layout);

    m_Plane.VAO->Bind();
    m_Plane.IBO = std::make_unique<IndexBuffer>(m_Indices.data(), static_cast<unsigned int>(m_Indices.size()));
    m_Plane.VAO->Unbind();
}

void TerrainChunk::Render()
{
    m_Plane.VAO->Bind();
    GLCall(glPatchParameteri(GL_PATCH_VERTICES, 4));
    GLCall(glDrawElements(GL_PATCHES, m_Plane.IBO->GetCount(), GL_UNSIGNED_INT, nullptr));
    m_Plane.VAO->Unbind();
}

glm::mat4 TerrainChunk::GetModelMatrix() const
{
    return glm::translate(glm::mat4(1.0f), glm::vec3(0.0f)); // No need to translate - already in world coords
}

AABB TerrainChunk::GetAABB(float maxHeight) const
{
    return {
        glm::vec3(m_X * m_ChunkSize, -maxHeight / 2, m_Z * m_ChunkSize),
        glm::vec3((m_X + 1) * m_ChunkSize, maxHeight / 2, (m_Z + 1) * m_ChunkSize)
    };
}

int TerrainChunk::GetResolution() const
{
    return m_Resolution;
}

std::pair<int, int> TerrainChunk::GetChunkCoordinates() const
{
    return { m_X, m_Z };
}