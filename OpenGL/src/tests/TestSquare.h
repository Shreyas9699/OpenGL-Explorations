#pragma once
#include "TestIncludeHeader.h"
#include "Window.h"
#include <memory>

namespace test
{
	class TestSquare : public Test
	{
	private:
		Window* m_Window;
		float m_Color[4];
		float m_aspectRatio;
		glm::mat4 m_Projection;
		std::unique_ptr<VertexArray> m_VA;
		std::unique_ptr<VertexBuffer> m_VB;
		std::unique_ptr<IndexBuffer> m_IB;
		std::unique_ptr<Shader> m_Shader;
		std::unique_ptr<Renderer> m_Renderer;

	public:
		TestSquare(Window* window);

		void OnUpdate(Timestep deltaTime, GLFWwindow* m_Window = nullptr) override;
		void OnRender() override;
		void OnImGuiRender() override;
	};
}