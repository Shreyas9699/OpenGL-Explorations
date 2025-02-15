#pragma once
#include "TestIncludeHeader.h"
#include "Sphere.h"
#include "../src/Window.h"
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
		Window* m_window;
		float m_planeColor[4];
		float lightColor[4];
		std::unique_ptr<UVSphere> m_Sphere;
		Object m_plane, m_sphere;
		std::unique_ptr<Shader> m_ShaderPlane;
		std::unique_ptr<Shader> m_ShaderLightSrc;
		Camera m_Camera;
		CameraController m_cameraController;
		std::unique_ptr<Renderer> m_Renderer;
		bool isRotating = false;
		bool isMoving = false;
		bool cusorEnable = false;
		float lastTtime = 0.0f;
		float horizontalrotationAngle = 0.0f;
		float rotationSpeed = 10.0f;

		glm::vec3 lightPos;

		void handleKeyPress(int key, int scancode, int action, int mods);

	public:
		TestFBMPlane(Window* win);
		~TestFBMPlane();


		void OnUpdate(Timestep deltaTime, GLFWwindow* win);
		void OnRender() override;
		void OnImGuiRender() override;
	};
}