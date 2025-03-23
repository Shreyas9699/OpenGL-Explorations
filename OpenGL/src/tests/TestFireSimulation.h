#pragma once
#include "TestIncludeHeader.h"
#include "Camera.h"
#include "CameraController.h"
#include "Window.h"

#include "ParticleSystemGPU.h"
#include "XZPlaneGrid.h"

namespace test
{
	class TestFireSimulation : public Test
	{

		Window* m_Window;
		std::unique_ptr<Shader> m_Shader;
		std::unique_ptr<Shader> m_ShaderGPU;
		std::unique_ptr<XZPlaneGrid> m_XZPlane;
		std::unique_ptr<ParticleSystemGPU> m_ParticleSys;
		Camera m_Camera;
		CameraController m_cameraController;
		int m_GridSize = 100;
		int m_GridSizePrev = 0;
		float m_scalingFactor = 1.0f;
		float m_Near = 0.01f;
		float m_Far = 10000.0f;
		bool isGridEnabled = false;
		bool cusorEnable = false;
		bool m_UseGPU = true;
		float m_ElapsedTime = 0.0f;

		// particle Emitter properties
		EmitterProperties emitterProp = EmitterProperties(glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(0.0f), EmitterShape::POINT, 2.0f, 45.0f, glm::vec3(2.0f), 1000);

		void handleKeyPress(int key, int scancode, int action, int mods);

	public:
		TestFireSimulation(Window* window);
		~TestFireSimulation();

		void OnUpdate(Timestep deltaTime, GLFWwindow* win);
		void OnRender() override;
		void OnImGuiRender() override;
	};
}