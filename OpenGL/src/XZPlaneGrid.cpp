#include "XZPlaneGrid.h"

XZPlaneGrid::XZPlaneGrid(float aspectRatio, size_t gridSize, float near, float far)
    : m_AspectRatio(aspectRatio),
      m_GridSize(gridSize),
      m_Near(near),
      m_Far(far)
{
    glGenVertexArrays(1, &m_VAO);
	m_ShaderGrid = std::make_unique<Shader>("res/shaders/infiniteGrid/InfiPlaneVertexShader.glsl", 
        "res/shaders/infiniteGrid/InfiPlaneFragmentShader.glsl");

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

XZPlaneGrid::~XZPlaneGrid()
{
	m_ShaderGrid.reset();
    m_VAO = 0;
	glDeleteVertexArrays(1, &m_VAO);
}

void XZPlaneGrid::UpdateAspectRation(float aspectRatio)
{
    m_AspectRatio = aspectRatio;
}

void XZPlaneGrid::UpdateGridSize(float gridSize)
{
    m_GridSize = gridSize;
}

void XZPlaneGrid::UpdateNear(float near)
{
    m_Near = near;
}

void XZPlaneGrid::UpdateFar(float far)
{
    m_Far = far;
}

void XZPlaneGrid::Render(const glm::mat4& view, const glm::mat4& projection)
{
    glDepthMask(GL_FALSE);
    m_ShaderGrid->Bind();
    glBindVertexArray(m_VAO);
    m_ShaderGrid->setMat4("projection", projection);
    m_ShaderGrid->setMat4("view", view);
    m_ShaderGrid->setFloat("near", m_Near);
    m_ShaderGrid->setFloat("far", m_Far);
    m_ShaderGrid->setFloat("gridSize", (float)m_GridSize);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
    m_ShaderGrid->Unbind();
    glDepthMask(GL_TRUE);
}
