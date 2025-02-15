#pragma once

#include "TestIncludeHeader.h"

#include <memory>

namespace test
{
	class TestSquare : public Test
	{
	private:
		float m_Color[4];
		std::unique_ptr<VertexArray> m_VA;
		std::unique_ptr<VertexBuffer> m_VB;
		std::unique_ptr<IndexBuffer> m_IB;
		std::unique_ptr<Shader> m_Shader;
		std::unique_ptr<Renderer> m_Renderer;

	public:
		TestSquare();
		~TestSquare();

		void OnUpdate(Timestep deltaTime, GLFWwindow* win = nullptr) override;
		void OnRender() override;
		void OnImGuiRender() override;
	};
}