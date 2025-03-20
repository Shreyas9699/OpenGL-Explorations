#pragma once
#include "TestIncludeHeader.h"
#include "Texture.h"
#include "Window.h"
#include "Frustum.h"
#include <stb_image/stb_image.h>
#include <filesystem>

namespace test
{
	struct AABB 
	{
		glm::vec3 min;
		glm::vec3 max;
	};

	class TestHeightMap : public Test
	{
		int temp = 1;
		Window* m_Window;
		GLint maxTessLevel;
		//unsigned int texture;
		//int m_nrChannels;
		int m_width = 0, m_height = 0;
		float m_Near = 0.1f, m_Far = 100000.0f;
		std::vector<float> vertices;
		std::unique_ptr<VertexArray> m_VA;
		std::unique_ptr<VertexBuffer> m_VB;
		std::unique_ptr<IndexBuffer> m_IB;
		std::unique_ptr<Shader> m_Shader;
		std::unique_ptr<Shader> m_NormalShader;
		std::unique_ptr<Texture> m_Texture;
		Camera m_Camera;
		CameraController m_cameraController;
		std::unique_ptr<Renderer> m_Renderer;
		bool enableFrustumCulling = false;
		bool isWireFrame = false;
		bool enableGrid = false;
		bool dynamicTess = false;
		bool cusorEnable = false;
		bool renderPointsOnly = false;
		bool showNormals = false;

		const unsigned int NUM_PATCH_PTS = 4;
		unsigned int rez = 20;

		Frustum m_Frustum;
		std::vector<AABB> m_PatchAABBs;
		std::vector<GLint> m_VisiblePatchStarts;
		float m_HeightScale = 100.0f; // Adjust based on your shader's height scale

		//std::vector<const char*> heightmaps = {};
		float errorMessageTime = 0.0f;
		std::string errorMessage;
		std::string texturePath;
		bool showFileExplorer = false;
		std::string selectedFile;
		std::string currentPath = std::filesystem::current_path().string();

		void handleKeyPress(int key, int scancode, int action, int mods);
		void ShowFileExplorer();
		void loadTexture();

		Camera m_BystanderCamera;
		std::unique_ptr<Shader> m_MiniMapShader;
		GLuint m_FrustumVAO, m_FrustumVBO;
		std::vector<glm::vec3> m_FrustumCorners;

		std::vector<GLint> ComputeVisiblePatchStarts();
		void RenderMiniMap();
		void RenderFrustum();

	public:
		TestHeightMap(Window* win);
		~TestHeightMap();

		void OnUpdate(Timestep deltaTime, GLFWwindow* window = nullptr) override;
		void OnRender() override;
		void OnImGuiRender() override;
	};
}