#include "TestClearColor.h"

#include "Renderer.h"
#include "imgui/imgui.h"

namespace test
{
	TestClearColor::TestClearColor()
		: m_ClearColor { 0.2f, 0.3f, 0.8f, 1.0f }
	{
	}

	TestClearColor::~TestClearColor()
	{
	}

	void TestClearColor::OnUpdate(Timestep deltaTime, GLFWwindow* m_Window)
	{}

	void TestClearColor::OnRender()
	{
		GLCall(glClearColor(m_ClearColor[0], m_ClearColor[1], m_ClearColor[2], m_ClearColor[3]));
		GLCall(glClear(GL_COLOR_BUFFER_BIT));
	}

	void TestClearColor::OnImGuiRender()
	{
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		ImGui::Text("FPS: %.1f (%.3f ms)", io.Framerate, 1000.0f / io.Framerate);
		ImGui::ColorEdit4("Clear Color", m_ClearColor);
	}
}