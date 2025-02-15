#include "TestSquare.h"

#include "imgui/imgui.h"

namespace test
{
	TestSquare::TestSquare()
		: m_Color{ 0.0f, 0.0f, 1.0f, 1.0f }
	{
		float positions[8] = { -0.5f, -0.5f, 0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f };
		unsigned int indices[6] = { 0, 1, 2, 0, 2, 3 };

		m_VA = std::make_unique<VertexArray>();
		m_VB = std::make_unique<VertexBuffer>(positions, sizeof(positions) * sizeof(float));
		VertexBufferLayout layout;
		layout.Push<float>(2);
		m_VA->AddBuffer(*m_VB, layout);

		m_IB = std::make_unique<IndexBuffer>(indices, sizeof(indices));
		m_Renderer = std::make_unique<Renderer>();

		m_Shader = std::make_unique<Shader>("res/shaders/QuadShaderVS.shader","res/shaders/QuadShaderFS.shader");
		m_Shader->Bind();
		m_Shader->setVec4("u_Color", m_Color[0], m_Color[1], m_Color[2], m_Color[3]);
	}

	TestSquare::~TestSquare()
	{
		m_VA.reset();
		m_VB.reset();
		m_IB.reset();
		m_Shader->Unbind();
	}

	void TestSquare::OnUpdate(Timestep deltaTime, GLFWwindow* win) {}

	void TestSquare::OnRender()
	{
		m_Shader->Bind();
		m_Shader->setVec4("u_Color", m_Color[0], m_Color[1], m_Color[2], m_Color[3]);
		m_Renderer->Draw(*m_VA, *m_IB, *m_Shader);
	}

	void TestSquare::OnImGuiRender()
	{
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		ImGui::Text("FPS: %.1f (%.3f ms)", io.Framerate, 1000.0f / io.Framerate);
		ImGui::ColorEdit4("Change Color", m_Color);
	}
}
