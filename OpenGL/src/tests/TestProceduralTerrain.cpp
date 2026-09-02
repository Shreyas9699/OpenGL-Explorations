#include "TestProceduralTerrain.h"
#include "imgui/imgui.h"
#include <iostream>

#define STB_PERLIN_IMPLEMENTATION
#include "stb/stb_perlin.h"
#include <set>

namespace test
{
	TestProceduralTerrain::TestProceduralTerrain(Window* win)
		: m_Window(win),
		  m_Camera(glm::vec3(0.0f, 10.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), 45.0f, -90.0f),
		  m_cameraController(m_Window->GetWindow(), m_Camera)
	{
		glEnable(GL_DEPTH_TEST);
		//glfwSetInputMode(m_window->getWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		m_Window->setCustomKeyCallback([this](int key, int scancode, int action, int mods)
			{
				this->handleKeyPress(key, scancode, action, mods);
			});

		m_Shader = std::make_unique<Shader>("res/shaders/terrain/vertexShader.glsl", 
			"res/shaders/terrain/fragmentShader.glsl",
			nullptr, 
			"res/shaders/terrain/TessellationControlShader.glsl", 
			"res/shaders/terrain/TessellationEvaluationShader.glsl"
		);

		m_Frustum = std::make_unique<Frustum>();
	}

	TestProceduralTerrain::~TestProceduralTerrain()
	{
		m_Window->setCustomKeyCallback(nullptr);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	void TestProceduralTerrain::handleKeyPress(int key, int scancode, int action, int mods)
	{
		if (key == GLFW_KEY_T && action == GLFW_PRESS)
		{
			// this flag is used to render the plane in wireframe mode
			isWireFrame = !isWireFrame;
		}
		if (key == GLFW_KEY_Q && action == GLFW_PRESS)
		{
			cusorEnable = !cusorEnable;
			if (cusorEnable)
			{
				glfwSetInputMode(m_Window->GetWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			}
			else
			{
				glfwSetInputMode(m_Window->GetWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			}
		}
	}

	void TestProceduralTerrain::UpdateChunks()
	{
		// Get camera position
		glm::vec3 cameraPos = m_Camera.Position;

		// Calculate camera-relative chunk coordinates
		int currentChunkX = static_cast<int>(std::floor(cameraPos.x / m_ChunkSize));
		int currentChunkZ = static_cast<int>(std::floor(cameraPos.z / m_ChunkSize));

		// Calculate adaptive view distance based on height
		float heightFactor = 1.0f + (cameraPos.y / m_heightMultiplier) * 0.2f;
		int adaptiveViewDistance = static_cast<int>(m_ViewDistance * heightFactor);
		adaptiveViewDistance = glm::clamp(adaptiveViewDistance, m_ViewDistance, m_MaxViewDistance);

		// Update frustum for culling
		glm::mat4 projection = glm::perspective(glm::radians(m_Camera.Zoom), m_cameraController.GetAspectRatio(), 0.1f, 10000.0f);
		glm::mat4 view = m_Camera.GetViewMatrix();
		m_Frustum->Update(projection * view);

		// Track which chunks we need this frame
		std::set<std::pair<int, int>> neededChunks;
		m_TotalChunks = 0;
		m_CulledChunks = 0;

		// Spiral outward from player chunk for more even loading
		for (int layer = 0; layer <= adaptiveViewDistance; layer++) 
		{
			for (int dx = -layer; dx <= layer; dx++) 
			{
				for (int dz = -layer; dz <= layer; dz++) 
				{
					// Only process the outer layer each iteration
					if (layer > 0 && abs(dx) < layer && abs(dz) < layer)
						continue;

					int x = currentChunkX + dx;
					int z = currentChunkZ + dz;
					m_TotalChunks++;

					// Create an AABB for the chunk to check visibility
					glm::vec3 min(x * m_ChunkSize, -m_heightMultiplier, z * m_ChunkSize);
					glm::vec3 max((x + 1) * m_ChunkSize, m_heightMultiplier, (z + 1) * m_ChunkSize);

					// Skip chunks outside the view frustum if culling is enabled
					if (m_EnableFrustumCulling && !m_Frustum->IsAABBVisible(min, max)) {
						m_CulledChunks++;
						continue;
					}

					neededChunks.insert({ x, z });
				}
			}
		}

		// Unload distant chunks
		auto it = m_Chunks.begin();
		while (it != m_Chunks.end()) 
		{
			if (neededChunks.find(it->first) == neededChunks.end()) 
			{
				auto toErase = it++;
				m_Chunks.erase(toErase);
			}
			else {
				++it;
			}
		}

		// Add new chunks
		for (const auto& chunk : neededChunks) 
		{
			if (m_Chunks.find(chunk) == m_Chunks.end()) 
			{
				// Calculate LOD level based on distance from camera
				int distX = abs(chunk.first - currentChunkX);
				int distZ = abs(chunk.second - currentChunkZ);
				int dist = std::max(distX, distZ);

				// Adaptive resolution based on distance and height
				int chunkResolution = CalculateChunkResolution(dist, cameraPos.y);

				m_Chunks[chunk] = std::make_unique<TerrainChunk>(chunk.first, chunk.second, m_ChunkSize, chunkResolution);
			}
		}

		m_VisibleChunks = neededChunks.size();
	}

	int TestProceduralTerrain::CalculateChunkResolution(int distanceFromCamera, float cameraHeight)
	{
		// Base resolution from the class member
		float baseResolution = static_cast<float>(m_Resolution);

		// Reduce resolution based on distance (farther = lower resolution)
		float distanceFactor = 1.0f - glm::clamp(static_cast<float>(distanceFromCamera) / m_MaxViewDistance, 0.0f, 0.9f);

		// Reduce resolution based on height (higher = lower resolution)
		float heightFactor = 1.0f - glm::clamp((cameraHeight - 10.0f) / 500.0f, 0.0f, 0.9f);

		// Combine factors and make sure we have at least 4 vertices per side
		int resolution = std::max(4, static_cast<int>(baseResolution * distanceFactor * heightFactor));

		// Make sure resolution is always a power of 2 + 1
		int power = static_cast<int>(log2(resolution - 1));
		return (1 << power) + 1;
	}

	void TestProceduralTerrain::OnUpdate(Timestep deltaTime, GLFWwindow* window)
	{
		m_cameraController.Update(deltaTime);

		UpdateChunks();
		
		// Update Shaders
		m_Shader->Bind();
		glm::mat4 projection = glm::perspective(glm::radians(m_Camera.Zoom), m_cameraController.GetAspectRatio(), 1.0f, 5000.0f);
		glm::mat4 view = m_Camera.GetViewMatrix();
		m_Shader->setVec2("u_resolution", { m_Window->GetWindowWidth(), m_Window->GetWindowHeight() });
		m_Shader->setMat4("view", view);
		m_Shader->setMat4("projection", projection);
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
		m_Shader->setMat4("model", model);
		m_Shader->setFloat("scale", m_Scale);
		m_Shader->setInt("seed", m_Seed);
		m_Shader->setInt("octaves", m_Octaves);
		m_Shader->setFloat("persistence", m_Persistence);
		m_Shader->setFloat("lacunarity", m_Lacunarity);
		m_Shader->setVec2("offset", m_Offset);
		m_Shader->setFloat("heightMultiplier", m_heightMultiplier);
		m_Shader->setFloat("wireframe", isWireFrame);
		m_Shader->setInt("noiseType", m_NoiseType);
		m_Shader->setFloat("seaLevel", sealevel); // Adjust for more or less water
		m_Shader->setFloat("islandDensity", islandDensity); // 0 = no islands, 1 = many islands
		m_Shader->setBool("enableIslands", enableIslands);
		m_Shader->setBool("enableLandmassColoring", enableLandmassColoring);
		m_Shader->Unbind();
	}

	void TestProceduralTerrain::OnRender()
	{
		GLCall(glClearColor(0.1f, 0.1f, 0.1f, 1.0f));
		GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

		m_Shader->Bind();
		if (isWireFrame)
		{
			GLCall(glPolygonMode(GL_FRONT_AND_BACK, GL_LINE));
		}
		else
		{
			GLCall(glPolygonMode(GL_FRONT_AND_BACK, GL_FILL));
		}
		for (auto& [coord, chunk] : m_Chunks) 
		{
			glm::mat4 model = chunk->GetModelMatrix();
			chunk->Render();
		}
		m_Shader->Unbind();
	}

	void TestProceduralTerrain::OnImGuiRender()
	{
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		ImGui::Text("FPS: %.1f (%.3f ms)", io.Framerate, 1000.0f / io.Framerate);

		ImGui::Text("Chunk Stats: Total %d / Visible %d / Culled %d", m_TotalChunks, m_VisibleChunks, m_CulledChunks);
		ImGui::Checkbox("Frustum Culling", &m_EnableFrustumCulling);

		static const char* noiseFunctionType[] = { "SimpleX", "Perlin" };
		ImGui::Combo("Terrain Type", &m_NoiseType, noiseFunctionType, IM_ARRAYSIZE(noiseFunctionType));

		ImGui::SliderFloat("Scale", &m_Scale, 0.3f, 20.0f, "%.2f");
		ImGui::SliderInt("Octaves", &m_Octaves, 0, 16);
		ImGui::SliderFloat("Persistence", &m_Persistence, 0.0f, 1.0f, "%.1f");
		if (ImGui::InputFloat("Lacunarity", &m_Lacunarity, 0.0f, 0.0f, "%.1f")) 
		{
			if (m_Lacunarity < 1.0f) 
			{
				m_Lacunarity = 1.0f;
			}
		}
		ImGui::SliderInt("Seed", &m_Seed, 0, 1000);
		ImGui::SliderFloat("Offset X", &m_Offset.x, 0.0f, 30.0f, "%.1f");
		ImGui::SliderFloat("Offset Y", &m_Offset.y, 0.0f, 30.0f, "%.1f"); //m_heightMultiplier
		ImGui::SliderFloat("Height Multiplier", &m_heightMultiplier, 0.0f, 10.0f, "%.2f");

		ImGui::Checkbox("Enable Islands", &enableIslands);
		ImGui::BeginDisabled(!enableIslands);
		ImGui::SliderFloat("Sea Level", &sealevel, 0.0f, 1.0f, "%.2f");
		ImGui::SliderFloat("Island Density", &islandDensity, 0.0f, 1.0f, "%.1f");
		ImGui::EndDisabled();

		ImGui::Checkbox("Enable Landmass Coloring", &enableLandmassColoring);

		ImGui::Text("Press below keys to enable/disable:\n");
		ImGui::Text("`T` -> Rendering Plane in Wireframe\n");
		ImGui::Text("Press `Q` to enable/disable Cursor (Mouse pointer)\n");
	}
}