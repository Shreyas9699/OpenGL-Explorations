#pragma once
#include "TestIncludeHeader.h"
#include "Camera.h"
#include "CameraController.h"
#include "Window.h"

#include "ParticleSystem.h"
#include "XZPlaneGrid.h"

namespace test
{
	class TestParticleCPU : public Test
	{
		Window* m_Window;
		std::unique_ptr<Shader> m_Shader;
		std::unique_ptr<XZPlaneGrid> m_XZPlane;
		std::unique_ptr<ParticleSystem> m_ParticleSys;
		Camera m_Camera;
		CameraController m_cameraController;
		int m_GridSize = 100;
		int m_GridSizePrev = 0;
		float m_Near = 0.01f;
		float m_Far = 1000.0f;
		bool isGridEnabled = false;
		bool cusorEnable = false;

		// particle Emitter properties
		std::vector<const char*> emitterShape;
		int currentEmitterShapeIdx = 0;
		EmitterProperties emitterProp = EmitterProperties(glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(0.0f), EmitterShape::POINT, 2.0f, 45.0f, glm::vec3(2.0f), 1000.0f);

		void handleKeyPress(int key, int scancode, int action, int mods);

	public:
		TestParticleCPU(Window* window);
		~TestParticleCPU();

		void OnUpdate(Timestep deltaTime, GLFWwindow* win);
		void OnRender() override;
		void OnImGuiRender() override;
	};
}