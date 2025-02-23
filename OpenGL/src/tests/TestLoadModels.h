#pragma once
#include "TestIncludeHeader.h"
#include "Camera.h"
#include "CameraController.h"
#include "Texture.h"
#include "Window.h"
#include "load_model_meshes.h"
#include <filesystem>

namespace test
{
	class TestLoadModels : public Test
	{
		Window* m_Window;
		std::unique_ptr<Shader> m_Shader;
		std::unique_ptr<Model> model_testing;
		Camera m_Camera;
		CameraController m_cameraController;
		bool isWireFrame = false;
		bool dynamicTess = false;
		bool cusorEnable = false;
		bool renderPointsOnly = false;
		bool showNormals = false;

		const unsigned int NUM_PATCH_PTS = 4;
		unsigned int rez = 20;

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