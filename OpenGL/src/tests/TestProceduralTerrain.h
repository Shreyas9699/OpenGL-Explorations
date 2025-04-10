#pragma once
#include "TestIncludeHeader.h"
#include "Frustum.h"
#include "Window.h"
#include "TerrainChunk.h"
#include <memory>

namespace test
{
	// Custom hash function for std::pair<int,int>
	struct PairHash
	{
		std::size_t operator()(const std::pair<int, int>& p) const
		{
			// Combine the hash of both integers
			return std::hash<int>{}(p.first) ^ (std::hash<int>{}(p.second) << 1);
		}
	};

	class TestProceduralTerrain : public Test
	{
	private:
		Window* m_Window;

		std::unordered_map<std::pair<int, int>, std::unique_ptr<TerrainChunk>, PairHash> m_Chunks;
		float m_ChunkSize = 100.0f;
		int m_ViewDistance = 2;
		int m_MaxViewDistance = 10;

		std::unique_ptr<Shader> m_Shader;
		Camera m_Camera;
		CameraController m_cameraController;

		std::unique_ptr<Frustum> m_Frustum;
		size_t m_TotalChunks = 0;
		int m_VisibleChunks = 0;
		int m_CulledChunks = 0;
		bool m_EnableFrustumCulling = true;

		// Grid resolution - this is key for terrain detail
		// Higher values create more detailed terrain but impact performance
		int m_Resolution = 100;			// Number of vertices per side
		float m_Scale = 5.0f;			// Scale of the FBM
		int m_Octaves = 4;				// Octaves of the FBM
		int m_Seed = 0;					// Seed of the FBM
		float m_Persistence = 0.5f;		// Persistence of the FBM
		float m_Lacunarity = 2.0f;		// Lacunarity of the FBM
		glm::vec2 m_Offset = glm::vec2(0.0f, 0.0f);
		float m_heightMultiplier = 4.0f;

		bool isWireFrame = false;
		bool cusorEnable = false;
		float lastTtime = 0.0f;
		int m_NoiseType = 0;
		float sealevel = 0.45f;
		float islandDensity = 0.3f;
		bool enableIslands = true;
		bool enableLandmassColoring = true;

		void handleKeyPress(int key, int scancode, int action, int mods);

	public:
		TestProceduralTerrain(Window* win);
		~TestProceduralTerrain();

		void UpdateChunks();
		void ComputeVisiblePatchStarts();

		void OnUpdate(Timestep deltaTime, GLFWwindow* window = nullptr) override;
		void OnRender() override;
		void OnImGuiRender() override;
	};
}