#pragma once
#include "TestIncludeHeader.h"
#include "Sphere.h"
#include <memory>

namespace test
{
	class TestUVSphere : public Test
	{
	private:
		float m_Color[4];
		std::unique_ptr<UVSphere> m_Sphere;
		std::unique_ptr<VertexArray> m_VA;
		std::unique_ptr<VertexBuffer> m_VB;
		std::unique_ptr<IndexBuffer> m_IB;
		std::unique_ptr<Shader> m_Shader;
		Camera m_Camera;
		CameraController m_cameraController;
		bool m_IsFilled = false;
		std::unique_ptr<Renderer> m_Renderer;
		glm::vec3 m_Position;

		float m_radius = 1.0f;
		unsigned int m_segment = 64;
		unsigned int m_rings = 32;
		float m_prevRadius = 1.0f;
		unsigned int m_prevSegment = 64;
		unsigned int m_prevRings = 32;
		bool sphereModified = false;
		const unsigned int minSegments = 16, maxSegments = 512;
		const unsigned int minRings = 8, maxRings = 256;

	public:
		TestUVSphere(GLFWwindow* window);
		~TestUVSphere();

		void OnUpdate(Timestep deltaTime, GLFWwindow* win = nullptr) override;
		void OnRender() override;
		void OnImGuiRender() override;
	};
}