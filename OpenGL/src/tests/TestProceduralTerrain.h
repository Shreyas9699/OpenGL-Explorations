#pragma once
#include "TestIncludeHeader.h"
#include "Sphere.h"
#include "Window.h"
#include <memory>

/*
* This is an application that implements Fractal Brownian Motion from the book of shaders
* Added light effects to this using Phong lighting model  (https://learnopengl.com/Lighting/Basic-Lighting)
*/

namespace test
{
	class TestProceduralTerrain : public Test
	{
	private:
		Window* m_Window;
		float m_planeColor[4];
		Object m_plane;
		std::unique_ptr<Shader> m_Shader;
		Camera m_Camera;
		CameraController m_cameraController;
		std::unique_ptr<Renderer> m_Renderer;

		float m_PlaneWidth = 10.0f;		// Default width
		float m_PlaneHeight = 10.0f;	// Default height (depth)
		float m_Scale = 0.3f;			// Scale of the FBM
		int m_Octaves = 4;				// Octaves of the FBM
		int m_Seed = 0;					// Seed of the FBM
		float m_Persistence = 0.5f;		// Persistence of the FBM
		float m_Lacunarity = 2.0f;		// Lacunarity of the FBM
		glm::vec2 m_Offset = glm::vec2(0.0f, 0.0f);

		std::vector<float> m_Vertices;
		std::vector<uint32_t> m_Indices;

		bool isRotating = false;
		bool isMoving = false;
		bool isWireFrame = false;
		bool cusorEnable = false;
		float lastTtime = 0.0f;
		float horizontalrotationAngle = 0.0f;
		float rotationSpeed = 10.0f;

		void handleKeyPress(int key, int scancode, int action, int mods);
		void GeneratePlane();
		void UpdatePlaneVertices();
	public:
		TestProceduralTerrain(Window* win);
		~TestProceduralTerrain();


		void OnUpdate(Timestep deltaTime, GLFWwindow* window = nullptr) override;
		void OnRender() override;
		void OnImGuiRender() override;
	};
}