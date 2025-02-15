#pragma once
#include <vector>

#define M_PI 3.14159265358979

class UVSphere
{
public:
    std::vector<float> m_Vertices;
    std::vector<unsigned int> m_Indices;
    unsigned int m_Segments;
    unsigned int m_Rings;
    float m_Radius;

    UVSphere(float radius = 1.0f, unsigned int segments = 32, unsigned int rings = 16)
        : m_Radius(radius), m_Segments(segments), m_Rings(rings)
    {
        Generate();
    }

    unsigned int GetVertexCount() const { return (m_Rings + 1) * (m_Segments + 1); }
    const std::vector<float>& GetVertex() const { return m_Vertices; }
    const std::vector<unsigned int>& GetIndex() const { return m_Indices; }

    void ReGenerate(float radius, unsigned int segments, unsigned int rings)
    {
        m_Radius = radius;
        m_Segments = segments;
        m_Rings = rings;
        Generate();
    }

private:
    void Generate() 
    {
        m_Vertices.clear();
        m_Indices.clear();

        for (unsigned int y = 0; y <= m_Rings; y++)
        {
            float phi = (float)y * M_PI / (float)m_Rings;
            float sinPhi = sin(phi);
            float cosPhi = cos(phi);

            for (unsigned int x = 0; x <= m_Segments; x++)
            {
                float theta = (float)x * 2.0f * M_PI / (float)m_Segments;
                float sinTheta = sin(theta);
                float cosTheta = cos(theta);

                // Position
                float xPos = cosTheta * sinPhi;
                float yPos = cosPhi;
                float zPos = sinTheta * sinPhi;

                // Add position
                m_Vertices.push_back(xPos * m_Radius);
                m_Vertices.push_back(yPos * m_Radius);
                m_Vertices.push_back(zPos * m_Radius);

                // Add normal (same as position for a sphere)
                m_Vertices.push_back(xPos);
                m_Vertices.push_back(yPos);
                m_Vertices.push_back(zPos);
            }
        }

        for (unsigned int y = 0; y < m_Rings; y++)
        {
            for (unsigned int x = 0; x < m_Segments; x++)
            {
                unsigned int current = y * (m_Segments + 1) + x;
                unsigned int next = current + m_Segments + 1;

                m_Indices.push_back(current);
                m_Indices.push_back(next);
                m_Indices.push_back(current + 1);

                m_Indices.push_back(next);
                m_Indices.push_back(next + 1);
                m_Indices.push_back(current + 1);
            }
        }
    }
};