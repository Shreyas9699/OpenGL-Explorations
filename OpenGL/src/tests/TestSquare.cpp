#include "TestSquare.h"

#include "imgui/imgui.h"

namespace test
{
	TestSquare::TestSquare()
		: m_Color{ 0.2f, 0.3f, 0.8f, 1.0f }
	{
		float positions[8] = { -0.5f, -0.5f, 0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f };
		unsigned int indices[6] = { 0, 1, 2, 2, 3, 0 };

		m_VA = std::make_unique<VertexArray>();
		m_VB = std::make_unique<VertexBuffer>(positions, 4 * 2 * sizeof(float));
		VertexBufferLayout layout;
		layout.Push<float>(2);
		m_VA->AddBuffer(*m_VB, layout);

		m_IB = std::make_unique<IndexBuffer>(indices, 6);

		m_Shader = std::make_unique<Shader>("res/shaders/QuadShader.shader");
		m_Shader->Bind();
		m_Shader->SetUniform4f("u_Color", m_Color[0], m_Color[1], m_Color[2], m_Color[3]);
	}

	TestSquare::~TestSquare()
	{
		m_Shader->Unbind();
	}

	void TestSquare::OnUpdate(Timestep deltaTime) {}

	void TestSquare::OnRender()
	{
		GLCall(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
		GLCall(glClear(GL_COLOR_BUFFER_BIT));

		Renderer rendere;
		m_Shader->Bind();
		m_Shader->SetUniform4f("u_Color", m_Color[0], m_Color[1], m_Color[2], m_Color[3]);
		rendere.Draw(*m_VA, *m_IB, *m_Shader);
	}

	void TestSquare::OnImGuiRender()
	{
		ImGui::ColorEdit4("Change Color", m_Color);
	}
}
