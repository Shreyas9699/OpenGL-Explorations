#pragma once
#include "TestIncludeHeader.h"
#include "Texture.h"
#include "Camera.h"
#include "CameraController.h"

#include <filesystem>

namespace test
{
	class TestPyramid : public Test
	{
	private:
		float m_Color[4];
		GLFWwindow* m_Window;
		Camera m_Camera;
		CameraController m_cameraController;
		std::unique_ptr<VertexArray> m_VA;
		std::unique_ptr<VertexBuffer> m_VB;
		std::unique_ptr<IndexBuffer> m_IB;
		std::unique_ptr<Texture> m_Texture;
		std::unique_ptr<Shader> m_Shader;
		std::unique_ptr<Renderer> m_Renderer;

	public:
		TestPyramid(GLFWwindow* window);
		~TestPyramid();

		void OnUpdate(Timestep deltaTime, GLFWwindow* win = nullptr) override;
		void OnRender() override;
		void OnImGuiRender() override;
	};
}