#pragma once
#include "TestIncludeHeader.h"
#include "Material.h"
#include "Sphere.h"
#include <memory>

namespace test
{
	class TestCubeRendering : public Test
	{
	private:
		float ObjectColor[4];
		std::unique_ptr<UVSphere> m_Sphere;
		Object m_cube, m_sphere;
		std::unique_ptr<Shader> m_ShaderCube;
		std::unique_ptr<Shader> m_ShaderLight;
		Camera m_Camera;
		CameraController m_cameraController;
		std::unique_ptr<Renderer> m_Renderer;
		glm::vec3 lightPosition = glm::vec3(1.0f, 1.0f, 1.0f);
		float m_LastX = 900.0f / 2.0f;
		float m_LastY = 900.0f / 2.0f;
		bool m_FirstMouse = true;

		Light lightProperties;
		MaterialProperty mat_prop;
		int current_material_index = 0;
		std::vector<const char*> material_names;

	public:
		TestCubeRendering(GLFWwindow* window);
		~TestCubeRendering();

		struct VertexWithNormal
		{
			glm::vec3 position;
			glm::vec3 normal;
		};

		void OnUpdate(Timestep deltaTime, GLFWwindow* m_Window = nullptr) override;
		void OnRender() override;
		void OnImGuiRender() override;
	};
}