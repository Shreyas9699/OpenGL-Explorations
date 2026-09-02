#include "TestHeightMap.h"
#include "imgui/imgui.h"
#include <iostream>

namespace test
{
	TestHeightMap::TestHeightMap(Window* win)
		: m_Window(win),
		  m_Camera( glm::vec3(0.0f, 100.0f, 100.0f),  glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f),  -20.0f, 0.0f, 100.0f),
		  m_cameraController(m_Window->GetWindow(), m_Camera),
		  m_Near(0.1f),
		  m_Far(10000.0f)
	{
		glEnable(GL_DEPTH_TEST);
		m_Window->setCustomKeyCallback([this](int key, int scancode, int action, int mods)
			{
				this->handleKeyPress(key, scancode, action, mods);
			});
		glfwSetInputMode(m_Window->GetWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

		glGetIntegerv(GL_MAX_TESS_GEN_LEVEL, &maxTessLevel);
		std::cout << "Max available tess level: " << maxTessLevel << std::endl;

		m_Shader = std::make_unique<Shader>("res/shaders/Heightmap/heightmapVS.glsl", "res/shaders/Heightmap/heightmapFS.glsl",
			nullptr, "res/shaders/Heightmap/heightmapTCS.glsl", "res/shaders/Heightmap/heightmapTES.glsl");

		m_NormalShader = std::make_unique<Shader>("res/shaders/Heightmap/heightmapVS.glsl", "res/shaders/Heightmap/heightmapNormalFS.glsl",
			"res/shaders/Heightmap/heightmapGS.glsl", "res/shaders/Heightmap/heightmapTCS.glsl", "res/shaders/Heightmap/heightmapTES.glsl");

		texturePath = "res/textures/height/deccan_heightmap.png";
		selectedFile = texturePath;
		loadTexture();

		glPatchParameteri(GL_PATCH_VERTICES, NUM_PATCH_PTS);

		m_BystanderCamera = Camera(
			glm::vec3(700.0f, 3200.0f, 1600.0f),
			glm::vec3(0.0f, 1.0f, 0.0f),
			glm::normalize(glm::vec3(-700.0f, -3200.0f, -1600.0f)),
			-115.0f,  // Yaw
			-65.0f    // Pitch
		);

		// Mini-map shader
		m_MiniMapShader = std::make_unique<Shader>("res/shaders/Heightmap/minimapVS.glsl", "res/shaders/Heightmap/minimapFS.glsl");

		// Frustum visualization VAO
		m_Frustum = std::make_unique<Frustum>();
		m_FrustumVAO = std::make_unique<VertexArray>();
		m_FrustumVBO = std::make_unique<VertexBuffer>(nullptr, static_cast<unsigned int>(NUM_FRUSTUM_FILL_VERTS * sizeof(glm::vec3)), GL_ARRAY_BUFFER, GL_DYNAMIC_DRAW);
		VertexBufferLayout layout;
		layout.Push<float>(3);
		m_FrustumVAO->AddBuffer(*m_FrustumVBO, layout);

		//glGenVertexArrays(1, &m_FrustumVAO);
		//glGenBuffers(1, &m_FrustumVBO);

		//glBindVertexArray(m_FrustumVAO);
		//glBindBuffer(GL_ARRAY_BUFFER, m_FrustumVBO);
		//// Allocate space for 36 vertices (12 triangles * 3 verts)
		//glBufferData(GL_ARRAY_BUFFER, NUM_FRUSTUM_FILL_VERTS * sizeof(glm::vec3), nullptr, GL_DYNAMIC_DRAW);
		//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
		//glEnableVertexAttribArray(0);

		//glBindBuffer(GL_ARRAY_BUFFER, 0);
		//glBindVertexArray(0);
	}

	TestHeightMap::~TestHeightMap()
	{
		m_Window->setCustomKeyCallback(nullptr);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	void TestHeightMap::loadTexture()
	{
		// clear the variables
		m_VA.reset();
		m_VB.reset();
		m_IB.reset();
		m_Texture.reset();

		// load texture
		m_Texture = std::make_unique<Texture>(texturePath.c_str());
		m_Texture->Bind();
		m_width = m_Texture->GetWidth();
		m_height = m_Texture->GetHeight();

		m_Shader->Bind();
		m_Shader->setInt("heightMap", 0);
		m_Shader->Unbind();

		vertices.clear();
		vertices.reserve(rez * rez * 20); // for each loop 3 xyz and 2 uv x 5 times => 20
		for (unsigned int i = 0; i < rez; i++)
		{
			for (unsigned int j = 0; j < rez; j++)
			{
				vertices.push_back(-m_width / 2.0f + m_width * i / (float)rez); // v.x
				vertices.push_back(0.0f); // v.y
				vertices.push_back(-m_height / 2.0f + m_height * j / (float)rez); // v.z
				vertices.push_back(i / (float)rez); // u
				vertices.push_back(j / (float)rez); // v

				vertices.push_back(-m_width / 2.0f + m_width * (i + 1) / (float)rez); // v.x
				vertices.push_back(0.0f); // v.y
				vertices.push_back(-m_height / 2.0f + m_height * j / (float)rez); // v.z
				vertices.push_back((i + 1) / (float)rez); // u
				vertices.push_back(j / (float)rez); // v

				vertices.push_back(-m_width / 2.0f + m_width * i / (float)rez); // v.x
				vertices.push_back(0.0f); // v.y
				vertices.push_back(-m_height / 2.0f + m_height * (j + 1) / (float)rez); // v.z
				vertices.push_back(i / (float)rez); // u
				vertices.push_back((j + 1) / (float)rez); // v

				vertices.push_back(-m_width / 2.0f + m_width * (i + 1) / (float)rez); // v.x
				vertices.push_back(0.0f); // v.y
				vertices.push_back(-m_height / 2.0f + m_height * (j + 1) / (float)rez); // v.z
				vertices.push_back((i + 1) / (float)rez); // u
				vertices.push_back((j + 1) / (float)rez); // v
			}
		}

		m_VA = std::make_unique<VertexArray>();
		m_VB = std::make_unique<VertexBuffer>(vertices.data(), static_cast<unsigned int>(vertices.size() * sizeof(float)));
		VertexBufferLayout layout;
		layout.Push<float>(3); // positions
		layout.Push<float>(2); // texCoord
		m_VA->AddBuffer(*m_VB, layout);

		// Generate patch AABBs
		m_PatchAABBs.clear();
		m_PatchAABBs.reserve(rez * rez);
		for (unsigned int i = 0; i < rez; ++i) 
		{
			for (unsigned int j = 0; j < rez; ++j) 
			{
				float x_start = -m_width / 2.0f + m_width * i / (float)rez;
				float x_end = x_start + m_width / (float)rez;
				float z_start = -m_height / 2.0f + m_height * j / (float)rez;
				float z_end = z_start + m_height / (float)rez;

				AABB aabb;
				aabb.min = glm::vec3(x_start, 0.0f, z_start);
				aabb.max = glm::vec3(x_end, m_HeightScale, z_end);
				m_PatchAABBs.push_back(aabb);
			}
		}
	}

	void TestHeightMap::handleKeyPress(int key, int scancode, int action, int mods)
	{
		if (key == GLFW_KEY_T && action == GLFW_PRESS)
		{
			isWireFrame = !isWireFrame;
		}
		if (key == GLFW_KEY_G && action == GLFW_PRESS)
		{
			enableGrid = !enableGrid;
		}
		if (key == GLFW_KEY_E && action == GLFW_PRESS)
		{
			dynamicTess = !dynamicTess;
		}
		if (key == GLFW_KEY_F && action == GLFW_PRESS)
		{
			enableFrustumCulling = !enableFrustumCulling;
		}
		if (key == GLFW_KEY_P && action == GLFW_PRESS)
		{
			renderPointsOnly = !renderPointsOnly;
		}
		if (key == GLFW_KEY_N && action == GLFW_PRESS)
		{
			showNormals = !showNormals;
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

	void TestHeightMap::OnUpdate(Timestep deltaTime, GLFWwindow* window)
	{
		m_cameraController.Update(deltaTime);
		if (texturePath != selectedFile)
		{
			texturePath = selectedFile;
			loadTexture();
		}
	}

	std::vector<GLint> TestHeightMap::ComputeVisiblePatchStarts()
	{
		std::vector<GLint> visiblePatches;
		for (size_t idx = 0; idx < m_PatchAABBs.size(); ++idx)
		{
			const auto& aabb = m_PatchAABBs[idx];
			if (m_Frustum->IsAABBVisible(aabb.min, aabb.max))
			{
				visiblePatches.push_back(static_cast<GLint>(idx * 4)); // 4 vertices per patch
			}
		}
		return visiblePatches;
	}

	void TestHeightMap::OnRender()
	{
		GLCall(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
		GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

		m_Shader->Bind();
		glm::mat4 projection = glm::perspective(glm::radians(m_Camera.Zoom), m_cameraController.GetAspectRatio(), m_Near, m_Far/4.0f);
		glm::mat4 view = m_Camera.GetViewMatrix();
		float aspect = m_cameraController.GetAspectRatio();
		float verticalFov = glm::radians(m_Camera.Zoom);
		float horizontalFov = 2.0f * atan(tan(verticalFov / 2.0f) * aspect);

		// Use the smaller FOV for conservative culling
		float fovCos = cos(std::min(verticalFov, horizontalFov));
		m_Shader->setFloat("fovCos", fovCos);
		m_Shader->setMat4("viewTCS", view); // Main camera's view for TCS
		m_Shader->setMat4("viewTES", view);
		m_Shader->setMat4("projection", projection);
		glm::mat4 model = glm::mat4(1.0f);
		m_Shader->setMat4("model", model);
		m_Shader->setInt("numGrids", (rez - 1) * (rez - 1));
		m_Shader->setBool("enableGrid", enableGrid);
		m_Shader->setBool("isDynamicTess", dynamicTess);
		m_Shader->setBool("showNormals", showNormals);
		m_Shader->setVec2("uTexelSize", { 1.0f/m_width, 1.0f/m_height});
		m_VA->Bind();
		if (renderPointsOnly)
		{
			glPointSize(3.0f);
			glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
		}

		if (isWireFrame)
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}

		if (!renderPointsOnly && !isWireFrame)
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}

		// Update frustum
		glm::mat4 ProjView = projection * view;
		m_Frustum->Update(ProjView);

		// Determine visible patches
		if (enableFrustumCulling)
		{
			m_VisiblePatchStarts.clear();
			m_VisiblePatchStarts = ComputeVisiblePatchStarts();

			if (!m_VisiblePatchStarts.empty())
			{
				std::vector<GLsizei> counts(m_VisiblePatchStarts.size(), 4);
				glMultiDrawArrays(GL_PATCHES, m_VisiblePatchStarts.data(), counts.data(), static_cast<unsigned int>(m_VisiblePatchStarts.size()));
			}
		}
		else
		{
			glDrawArrays(GL_PATCHES, 0, NUM_PATCH_PTS * rez * rez);
		}
		m_Shader->Unbind();

		if (showNormals)
		{
			m_NormalShader->Bind();
			m_NormalShader->setFloat("fovCos", fovCos);
			m_NormalShader->setBool("isDynamicTess", dynamicTess);
			m_NormalShader->setMat4("viewTCS", view); // Main camera's view for TCS
			m_NormalShader->setMat4("viewTES", view);
			m_NormalShader->setMat4("projection", projection);
			m_NormalShader->setMat4("model", model);
			m_NormalShader->setVec2("uTexelSize", { 1.0f / m_width, 1.0f / m_height });
			m_NormalShader->setFloat("normalLength", 2.0f); // Adjust length as needed

			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			if (enableFrustumCulling)
			{
				if (!m_VisiblePatchStarts.empty())
				{
					std::vector<GLsizei> counts(m_VisiblePatchStarts.size(), 4);
					glMultiDrawArrays(GL_PATCHES, m_VisiblePatchStarts.data(), counts.data(), static_cast<unsigned int>(m_VisiblePatchStarts.size()));
				}
			}
			else
			{
				glDrawArrays(GL_PATCHES, 0, NUM_PATCH_PTS * rez * rez);
			}
			m_NormalShader->Unbind();
		}

		RenderMiniMap();
	}

	void TestHeightMap::RenderMiniMap()
	{
		// Set viewport to top-right quarter
		int winWidth, winHeight;
		glfwGetWindowSize(m_Window->GetWindow(), &winWidth, &winHeight);
		glViewport(winWidth - winWidth / 4, winHeight - winHeight / 4, winWidth / 4, winHeight / 4);
		glScissor(winWidth - winWidth / 4, winHeight - winHeight / 4, winWidth / 4, winHeight / 4);
		glEnable(GL_SCISSOR_TEST);
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glDisable(GL_SCISSOR_TEST);

		m_Shader->Bind();
		glm::mat4 projection = glm::perspective(glm::radians(m_Camera.Zoom), m_cameraController.GetAspectRatio(), m_Near, m_Far);
		glm::mat4 view = m_Camera.GetViewMatrix();
		glm::mat4 bystanderProjection = glm::perspective(glm::radians(m_BystanderCamera.Zoom), (winWidth / 2.0f) / (float)winHeight, m_Near, m_Far * 2.0f);
		glm::mat4 bystanderView = m_BystanderCamera.GetViewMatrix();
		float aspect = m_cameraController.GetAspectRatio();
		float verticalFov = glm::radians(m_Camera.Zoom);
		float horizontalFov = 2.0f * atan(tan(verticalFov / 2.0f) * aspect);
		float fovCos = cos(std::min(verticalFov, horizontalFov));
		m_Shader->setFloat("fovCos", fovCos);
		m_Shader->setMat4("viewTCS", view); // Main camera's view for TCS
		m_Shader->setMat4("viewTES", bystanderView);
		m_Shader->setMat4("projection", projection);
		glm::mat4 model = glm::mat4(1.0f);
		m_Shader->setMat4("model", model);
		m_Shader->setInt("numGrids", (rez - 1) * (rez - 1));
		m_Shader->setBool("enableGrid", enableGrid);
		m_Shader->setBool("isDynamicTess", dynamicTess);
		m_Shader->setBool("showNormals", showNormals);
		m_Shader->setVec2("uTexelSize", { 1.0f / m_width, 1.0f / m_height });
		m_VA->Bind();
		if (renderPointsOnly)
		{
			glPointSize(3.0f);
			glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
		}

		if (isWireFrame)
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}

		if (!renderPointsOnly && !isWireFrame)
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}

		// Update frustum
		m_Frustum->Update(projection * view);

		// Determine visible patches
		if (enableFrustumCulling)
		{
			m_VisiblePatchStarts.clear();
			m_VisiblePatchStarts = ComputeVisiblePatchStarts();

			if (!m_VisiblePatchStarts.empty())
			{
				std::vector<GLsizei> counts(m_VisiblePatchStarts.size(), 4);
				glMultiDrawArrays(GL_PATCHES, m_VisiblePatchStarts.data(), counts.data(), static_cast<unsigned int>(m_VisiblePatchStarts.size()));
			}
		}
		else
		{
			glDrawArrays(GL_PATCHES, 0, NUM_PATCH_PTS * rez * rez);
		}
		m_Shader->Unbind();
		RenderFrustum();
	}

	void TestHeightMap::RenderFrustum()
	{
		int winWidth, winHeight;
		glfwGetWindowSize(m_Window->GetWindow(), &winWidth, &winHeight);

		float fovRadians = glm::radians(m_Camera.Zoom);
		float aspectMain = (winWidth / 2.0f) / (float)winHeight;

		float nearHeight = 2.0f * tan(fovRadians / 2.0f) * m_Near;
		float nearWidth = nearHeight * aspectMain;
		float farHeight = 2.0f * tan(fovRadians / 2.0f) * m_Far;
		float farWidth = farHeight * aspectMain;

		// 2) Camera basis
		glm::vec3 camPos = m_Camera.Position;
		glm::vec3 camForward = glm::normalize(m_Camera.Front);
		glm::vec3 camRight = glm::normalize(glm::cross(camForward, m_Camera.Up));
		glm::vec3 camUp = glm::normalize(glm::cross(camRight, camForward));

		glm::vec3 nearCenter = camPos + camForward * m_Near;
		glm::vec3 farCenter = camPos + camForward * m_Far;

		// 3) Corner points
		glm::vec3 nearTopLeft = nearCenter + (camUp * (nearHeight * 0.5f)) - (camRight * (nearWidth * 0.5f));
		glm::vec3 nearTopRight = nearCenter + (camUp * (nearHeight * 0.5f)) + (camRight * (nearWidth * 0.5f));
		glm::vec3 nearBottomLeft = nearCenter - (camUp * (nearHeight * 0.5f)) - (camRight * (nearWidth * 0.5f));
		glm::vec3 nearBottomRight = nearCenter - (camUp * (nearHeight * 0.5f)) + (camRight * (nearWidth * 0.5f));

		glm::vec3 farTopLeft = farCenter + (camUp * (farHeight * 0.5f)) - (camRight * (farWidth * 0.5f));
		glm::vec3 farTopRight = farCenter + (camUp * (farHeight * 0.5f)) + (camRight * (farWidth * 0.5f));
		glm::vec3 farBottomLeft = farCenter - (camUp * (farHeight * 0.5f)) - (camRight * (farWidth * 0.5f));
		glm::vec3 farBottomRight = farCenter - (camUp * (farHeight * 0.5f)) + (camRight * (farWidth * 0.5f));

		std::vector<glm::vec3> frustumTriangles(NUM_FRUSTUM_FILL_VERTS);
		int idx = 0;

		// Near plane (two triangles)
		frustumTriangles[idx++] = nearTopLeft;
		frustumTriangles[idx++] = nearBottomLeft;
		frustumTriangles[idx++] = nearBottomRight;

		frustumTriangles[idx++] = nearTopLeft;
		frustumTriangles[idx++] = nearBottomRight;
		frustumTriangles[idx++] = nearTopRight;

		// Far plane (two triangles)
		frustumTriangles[idx++] = farTopLeft;
		frustumTriangles[idx++] = farTopRight;
		frustumTriangles[idx++] = farBottomRight;

		frustumTriangles[idx++] = farTopLeft;
		frustumTriangles[idx++] = farBottomRight;
		frustumTriangles[idx++] = farBottomLeft;

		// Left plane (two triangles)
		frustumTriangles[idx++] = nearTopLeft;
		frustumTriangles[idx++] = farBottomLeft;
		frustumTriangles[idx++] = nearBottomLeft;

		frustumTriangles[idx++] = nearTopLeft;
		frustumTriangles[idx++] = farTopLeft;
		frustumTriangles[idx++] = farBottomLeft;

		// Right plane (two triangles)
		frustumTriangles[idx++] = nearTopRight;
		frustumTriangles[idx++] = nearBottomRight;
		frustumTriangles[idx++] = farBottomRight;

		frustumTriangles[idx++] = nearTopRight;
		frustumTriangles[idx++] = farBottomRight;
		frustumTriangles[idx++] = farTopRight;

		// Top plane (two triangles)
		frustumTriangles[idx++] = nearTopLeft;
		frustumTriangles[idx++] = farTopLeft;
		frustumTriangles[idx++] = farTopRight;

		frustumTriangles[idx++] = nearTopLeft;
		frustumTriangles[idx++] = farTopRight;
		frustumTriangles[idx++] = nearTopRight;

		// Bottom plane (two triangles)
		frustumTriangles[idx++] = nearBottomLeft;
		frustumTriangles[idx++] = farBottomRight;
		frustumTriangles[idx++] = farBottomLeft;

		frustumTriangles[idx++] = nearBottomLeft;
		frustumTriangles[idx++] = nearBottomRight;
		frustumTriangles[idx++] = farBottomRight;

		// 5) Upload data to the fill VBO
		m_FrustumVBO->Bind();
		m_FrustumVBO->UpdateData(frustumTriangles.data(), NUM_FRUSTUM_FILL_VERTS * sizeof(glm::vec3));
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::vec3) * frustumTriangles.size(), frustumTriangles.data());
		m_FrustumVBO->Unbind();

		glm::mat4 bystanderProjection = glm::perspective(glm::radians(m_BystanderCamera.Zoom), (winWidth / 2.0f) / (float)winHeight, m_Near, m_Far);
		glm::mat4 bystanderView = m_BystanderCamera.GetViewMatrix();
		m_MiniMapShader->Bind();
		m_MiniMapShader->setMat4("projection", bystanderProjection);
		m_MiniMapShader->setMat4("view", bystanderView);
		m_MiniMapShader->setVec4("u_color", glm::vec4(0.0f, 1.0f, 0.0f, 0.3f));

		m_FrustumVAO->Bind();
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(frustumTriangles.size()));
		glDisable(GL_BLEND);
		m_FrustumVAO->Unbind();
		m_MiniMapShader->Unbind();
		glViewport(0, 0, winWidth, winHeight);
	}

	void TestHeightMap::ShowFileExplorer()
	{
		if (!showFileExplorer) return;

		std::string rootPath = "res/textures/height";
		std::filesystem::path rootFsPath = std::filesystem::absolute(rootPath);
		ImGui::Separator();

		if (std::filesystem::absolute(currentPath).string().find(rootFsPath.string()) == std::string::npos)
		{
			currentPath = rootPath;
		}

		ImGui::Text("Current Path: %s", currentPath.c_str());

		if (ImGui::Button("Cancel"))
		{
			showFileExplorer = false;
		}

		ImGui::Separator();

		for (const auto& entry : std::filesystem::directory_iterator(currentPath))
		{
			std::string name = entry.path().filename().string();
			std::string fullPath = entry.path().string();
			std::replace(fullPath.begin(), fullPath.end(), '\\', '/');
			bool isDirectory = entry.is_directory();

			if (isDirectory)
			{
				if (ImGui::Selectable((name + "/").c_str(), false))
				{
					currentPath = entry.path().string();
				}
			}
			else
			{
				if (name.find("_heightmap.png") != std::string::npos)
				{
					// Check if this is the currently selected file
					bool isSelected = (fullPath == selectedFile);

					// Use different colors for selected/unselected items
					if (isSelected)
					{
						ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 0.0f, 1.0f));  // Yellow for selected
					}

					if (ImGui::Selectable(name.c_str(), isSelected))
					{
						selectedFile = fullPath;
						std::cout << "Selected file: " << selectedFile << std::endl;
						showFileExplorer = false;
					}

					if (isSelected)
					{
						ImGui::PopStyleColor();
					}

					// Optional: Add a tooltip for the selected file
					if (isSelected && ImGui::IsItemHovered())
					{
						ImGui::BeginTooltip();
						ImGui::Text("Currently selected file");
						ImGui::EndTooltip();
					}
				}
			}
		}
		ImGui::Separator();
	}

	void TestHeightMap::OnImGuiRender()
	{
		ImVec4 enabledColor = ImVec4(0.0f, 1.0f, 0.0f, 1.0f);  // Green
		ImVec4 disabledColor = ImVec4(1.0f, 0.0f, 0.0f, 1.0f); // Red
		ImVec4 color;
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		ImGui::Text("FPS: %.1f (%.3f ms)", io.Framerate, 1000.0f / io.Framerate);

		if (ImGui::Button("Select Image"))
		{
			showFileExplorer = true;
		}
		ImGui::SameLine();
		ImGui::Text("Current heightmap: %s",
			(texturePath.substr(
				(texturePath.find_last_of("/\\") != std::string::npos) ?
				texturePath.find_last_of("/\\") + 1
				: 0)).c_str());
		if (showFileExplorer)
		{
			ShowFileExplorer();
		}
		ImGui::Text("Press below keys to enable/disable:\n");

		color = cusorEnable ? enabledColor : disabledColor;
		ImGui::Text("`Q` -> Cursor (Mouse pointer) ->");
		ImGui::SameLine();
		ImGui::TextColored(color, " [%s]\n", (cusorEnable) ? "ENABLED" : "DISABLED");

		color = enableFrustumCulling ? enabledColor : disabledColor;
		ImGui::Text("`F` -> Frustum Culling ->");
		ImGui::SameLine();
		ImGui::TextColored(color, " [%s]\n", (enableFrustumCulling) ? "ENABLED" : "DISABLED");

		color = dynamicTess ? enabledColor : disabledColor;
		ImGui::Text("`E` -> Dynamic Tessellation ->");
		ImGui::SameLine();
		ImGui::TextColored(color, " [%s]\n", (dynamicTess) ? "ENABLED" : "DISABLED");

		color = renderPointsOnly ? enabledColor : disabledColor;
		ImGui::Text("`P` -> Tessellated Points ->");
		ImGui::SameLine();
		ImGui::TextColored(color, " [%s]\n", (renderPointsOnly) ? "ENABLED" : "DISABLED");

		color = isWireFrame ? enabledColor : disabledColor;
		ImGui::Text("`T` -> WireFrame View  ->");
		ImGui::SameLine();
		ImGui::TextColored(color, " [%s]\n", (isWireFrame) ? "ENABLED" : "DISABLED");

		color = showNormals ? enabledColor : disabledColor;
		ImGui::Text("`N` -> Normal ->");
		ImGui::SameLine();
		ImGui::TextColored(color, " [%s]\n", (showNormals) ? "ENABLED" : "DISABLED");

		color = enableGrid ? enabledColor : disabledColor;
		ImGui::Text("`G` -> Patch Grid ->");
		ImGui::SameLine();
		ImGui::TextColored(color, " [%s]\n", (enableGrid) ? "ENABLED" : "DISABLED");

		color = ImVec4(0.2f, 0.6f, 0.8f, 1.0f);
		ImGui::TextColored(color, "If you have both Tessellation Points and WireFrame enabled\nThe Wireframe Color is override by points\n");
		ImGui::Text("Press `Backspace` to go back to main menu\n");
	}
}