#include "TestTriangle.h"
#include "imgui/imgui.h"

namespace test
{
	TestTriangle::TestTriangle()
		: m_Color{ 0.2f, 0.3f, 0.8f, 1.0f }
	{   
		float positions[6] = { -0.5f, -0.5f, 0.5f, -0.5f, 0.0f, 0.5f };
		unsigned int indices[3] = { 0, 1, 2 };

		m_VA = std::make_unique<VertexArray>();
		m_VB = std::make_unique<VertexBuffer>(positions, 3 * 2 * sizeof(float));
		VertexBufferLayout layout;
		layout.Push<float>(2);
		m_VA->AddBuffer(*m_VB, layout);

		m_IB = std::make_unique<IndexBuffer>(indices, 3);

		m_Shader = std::make_unique<Shader>("res/shaders/BasicShader.shader");
		m_Shader->Bind();
		m_Shader->SetUniform4f("u_Color", m_Color[0], m_Color[1], m_Color[2], m_Color[3]);
	}

	TestTriangle::~TestTriangle()
	{
		m_Shader->Unbind();
	}

	void TestTriangle::OnUpdate(Timestep deltaTime) {}

	void TestTriangle::OnRender()
	{
		GLCall(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
		GLCall(glClear(GL_COLOR_BUFFER_BIT));

		Renderer rendere;
		m_Shader->Bind();
		m_Shader->SetUniform4f("u_Color", m_Color[0], m_Color[1], m_Color[2], m_Color[3]);
		rendere.Draw(*m_VA, *m_IB, *m_Shader);
	}

	void TestTriangle::OnImGuiRender()
	{
		ImGui::ColorEdit4("Change Color", m_Color);
	}
}
