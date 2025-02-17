#pragma once
#include "TestIncludeHeader.h"
#include "Camera.h"
#include "CameraController.h"
#include "Texture.h"
#include "../src/Window.h"
#include <stb_image/stb_image.h>
#include <filesystem>

namespace fs = std::filesystem;

namespace test
{
	class TestHeightMap : public Test
	{
		Window* m_window;
		GLint maxTessLevel;
		//unsigned int texture;
		//int m_nrChannels;
		int m_width = 0, m_height = 0;
		std::vector<float> vertices;
		std::unique_ptr<VertexArray> m_VA;
		std::unique_ptr<VertexBuffer> m_VB;
		std::unique_ptr<IndexBuffer> m_IB;
		std::unique_ptr<Shader> m_Shader;
		std::unique_ptr<Texture> m_Texture;
		Camera m_Camera;
		CameraController m_cameraController;
		std::unique_ptr<Renderer> m_Renderer;
		bool isWireFrame = false;
		bool enableGrid = false;
		bool dynamicTess = false;
		bool cusorEnable = false;
		bool renderPointsOnly = false;
		bool showNormals = false;

		const unsigned int NUM_PATCH_PTS = 4;
		unsigned int rez = 20;

		//std::vector<const char*> heightmaps = {};
		float errorMessageTime = 0.0f;
		std::string errorMessage;
		std::string texturePath;
		bool showFileExplorer = false;
		std::string selectedFile;
		std::string currentPath = fs::current_path().string();

		void handleKeyPress(int key, int scancode, int action, int mods);
		void ShowFileExplorer();
		void loadTexture();

	public:
		TestHeightMap(Window* window);
		~TestHeightMap();

		void OnUpdate(Timestep deltaTime, GLFWwindow* win);
		void OnRender() override;
		void OnImGuiRender() override;
	};
}