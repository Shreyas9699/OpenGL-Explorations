#include "TestTriangle.h"
#include "Random.h"
#include "imgui/imgui.h"
#define _USE_MATH_DEFINES
#include <cmath>

namespace test
{
	TestTriangle::TestTriangle(Window* window)
		: m_Window(window), m_Color{ 0.0f, 0.0f, 1.0f, 1.0f }, m_aspectRatio(m_Window->GetAspectRatio()),
		  m_Projection(glm::mat4(0.0f))
	{   
		float positions[] = { -0.5f, -0.5f, 0.5f, -0.5f, 0.0f, 0.5f };
		unsigned int indices[] = { 0, 1, 2 };
		m_VA = std::make_unique<VertexArray>();
		m_VB = std::make_unique<VertexBuffer>(positions, static_cast<unsigned int>(sizeof(positions)));
		VertexBufferLayout layout;
		layout.Push<float>(2);
		m_VA->AddBuffer(*m_VB, layout);

		m_IB = std::make_unique<IndexBuffer>(indices, static_cast<unsigned int>(sizeof(indices)));
		m_Renderer = std::make_unique<Renderer>();

		m_Shader = std::make_unique<Shader>("res/shaders/Basic/BasicShaderVS.glsl", "res/shaders/Basic/BasicShaderFS.glsl");
	}

	void TestTriangle::OnUpdate(Timestep deltaTime, GLFWwindow* win)
	{
		m_aspectRatio = m_Window->GetAspectRatio();
		m_Projection = glm::ortho(-m_aspectRatio, m_aspectRatio, -1.0f, 1.0f, -1.0f, 1.0f);
		
		m_Timer += deltaTime;

		if (m_Timer > 1.0f)
		{
			m_Timer = 0.0f;
			m_VA.reset();
			m_VB.reset();

			float centerX = Random::Float(-m_aspectRatio, m_aspectRatio);
			float centerY = Random::Float(-1.0f, 1.0f);
			float r = Random::Float(0.2f, std::min(m_aspectRatio, 1.0f) * 2.0f);
			
			float angle = 0.0f;
			float positions[6];

			for (int i = 0; i < 3; i++)
			{
				angle += Random::Float(0.1f, 0.8f) * Random::PI;
				float x = centerX + r * glm::cos(angle);
				float y = centerY + r * glm::sin(angle);

				// Clamp x-coordinate to window boundaries
				if (x < -m_aspectRatio)
					x = -m_aspectRatio;
				else if (x > m_aspectRatio)
					x = m_aspectRatio;

				// Clamp y-coordinate to window boundaries
				if (y < -1.0f)
					y = -1.0f;
				else if (y > 1.0f)
					y = 1.0f;

				positions[i * 2] = x;
				positions[i * 2 + 1] = y;
			}

			m_VA = std::make_unique<VertexArray>();
			m_VB = std::make_unique<VertexBuffer>(positions, static_cast<unsigned int>(sizeof(positions)));
			VertexBufferLayout layout;
			layout.Push<float>(2);
			m_VA->AddBuffer(*m_VB, layout);
		}
	}

	void TestTriangle::OnRender()
	{
		m_Shader->Bind();
		m_Shader->setMat4("u_Projection", m_Projection);
		m_Shader->setVec4("u_Color", m_Color[0], m_Color[1], m_Color[2], m_Color[3]);
		m_Renderer->Draw(*m_VA, *m_IB, *m_Shader);
	}

	void TestTriangle::OnImGuiRender()
	{
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		ImGui::Text("FPS: %.1f (%.3f ms)", io.Framerate, 1000.0f / io.Framerate);
		ImGui::Text("Aspect Ratio: %.2f", m_aspectRatio);
		ImGui::ColorEdit4("Change Color", m_Color);

	}
}
