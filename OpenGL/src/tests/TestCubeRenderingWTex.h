#pragma once
#include "TestIncludeHeader.h"
#include "Camera.h"
#include "CameraController.h"
#include "Texture.h"
#include <glm/gtc/type_ptr.hpp>
#include <stb_image/stb_image.h>
#include <memory>

namespace test
{
	class TestCubeWithTex : public Test
	{
	private:
		float ObjectColor[4];
		std::vector<glm::vec3> cubePositions;
		std::unique_ptr<VertexArray> m_VA;
		std::unique_ptr<VertexBuffer> m_VB;
		std::unique_ptr<IndexBuffer> m_IB;
		std::unique_ptr<Texture> m_TextureDiffuse, m_TextureSpecular, m_EmissionMap;
		std::unique_ptr<Shader> m_ShaderCube;
		std::unique_ptr<Shader> m_ShaderLight;
		Camera m_Camera;
		CameraController m_cameraController;
		std::unique_ptr<Renderer> m_Renderer;
		glm::vec3 lightPosition = glm::vec3(1.0f, 1.0f, 2.0f);
		float m_LastX = 900.0f / 2.0f;
		float m_LastY = 900.0f / 2.0f;
		bool m_FirstMouse = true;
		bool m_Attenuation = false;

		PointLight lightProperties;

	public:
		TestCubeWithTex(GLFWwindow* window);
		~TestCubeWithTex();

		void OnUpdate(Timestep deltaTime, GLFWwindow* m_Window = nullptr) override;
		void OnRender() override;
		void OnImGuiRender() override;
	};
}