#pragma once
#include "TestIncludeHeader.h"
#include "Sphere.h"
#include "Window.h"
#include <memory>

/*
* This is an application that implements Fractal Brownian Motion from the book of shaders
* Added ligh effets to this using Phong lighting model (https://learnopengl.com/Lighting/Basic-Lighting)
*/

namespace test
{
	class TestFBMPlane : public Test
	{
	private:
		Window* m_Window;
		float m_planeColor[4];
		float lightColor[4];
		std::unique_ptr<UVSphere> m_Sphere;
		Object m_plane, m_sphere;
		std::unique_ptr<Shader> m_ShaderPlane;
		std::unique_ptr<Shader> m_ShaderLightSrc;
		Camera m_Camera;
		CameraController m_cameraController;
		std::unique_ptr<Renderer> m_Renderer;

		int m_GridSize = 50;
		float m_PlaneSize = 5.0f;
		int m_GridSizePrev;
		float m_PlaneSizePrev;
		std::vector<float> m_Vertices;
		std::vector<uint32_t> m_Indices;

		bool isRotating = false;
		bool isMoving = false;
		bool isWireFrame = false;
		bool cusorEnable = false;
		float lastTtime = 0.0f;
		float horizontalrotationAngle = 0.0f;
		float rotationSpeed = 10.0f;

		glm::vec3 lightPos;

		void handleKeyPress(int key, int scancode, int action, int mods);
		void GeneratePlane();
		void SliderIntEvent(const char* label, int* value, int min, int max, int step);
	public:
		TestFBMPlane(Window* win);
		~TestFBMPlane();


		void OnUpdate(Timestep deltaTime, GLFWwindow* window = nullptr) override;
		void OnRender() override;
		void OnImGuiRender() override;
	};
}