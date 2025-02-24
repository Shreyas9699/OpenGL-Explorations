#pragma once
#include "TestIncludeHeader.h"
#include "Camera.h"
#include "CameraController.h"
#include "Texture.h"
#include "Window.h"
#include "Model.h"
#include <filesystem>

namespace test
{
	class TestLoadModels : public Test
	{
		Window* m_Window;
		std::unique_ptr<Shader> m_Shader;
		std::unique_ptr<Shader> m_ShaderNormal;
		std::unique_ptr<Model> m_Model;
		Camera m_Camera;
		CameraController m_cameraController;
		bool isWireFrame = false;
		bool dynamicTess = false;
		bool cusorEnable = false;
		bool renderPointsOnly = false;
		bool showNormals = false;

		//std::vector<const char*> heightmaps = {};
		float errorMessageTime = 0.0f;
		std::string errorMessage;
		std::string currModelPath;
		bool showFileExplorer = false;
		std::string selectedModel;
		std::string currentPath = std::filesystem::current_path().string();

		void handleKeyPress(int key, int scancode, int action, int mods);
		void ShowFileExplorer();

	public:
		TestLoadModels(Window* window);
		~TestLoadModels();

		void OnUpdate(Timestep deltaTime, GLFWwindow* win);
		void OnRender() override;
		void OnImGuiRender() override;
	};
}