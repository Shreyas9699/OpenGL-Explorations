#include "TestHeightMap.h"
#include "imgui/imgui.h"
#include <iostream>

namespace test
{
	TestHeightMap::TestHeightMap(Window* win)
		: m_Window(win),
		  m_Camera(glm::vec3(0.0f, 50.0f, 100.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), -20.0f, 0.0f, 100.0f),
		  m_cameraController(m_Window->GetWindow(), m_Camera)
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

		m_NormalShader = std::make_unique<Shader>("res/shaders/Heightmap/heightmapVS.glsl", "res/shaders/Heightmap/heightmapFS.glsl",
			"res/shaders/Heightmap/heightmapGS.glsl", "res/shaders/Heightmap/heightmapTCS.glsl", "res/shaders/Heightmap/heightmapTES.glsl");

		texturePath = "res/textures/height/deccan_heightmap.png";
		selectedFile = texturePath;
		loadTexture();

		glPatchParameteri(GL_PATCH_VERTICES, NUM_PATCH_PTS);
	}

	TestHeightMap::~TestHeightMap()
	{
		m_VA.reset();
		m_VB.reset();
		m_IB.reset();
		m_Texture.reset();
		m_Shader->Unbind();
		m_Shader.reset();
		m_Renderer.reset();

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
		m_VB = std::make_unique<VertexBuffer>(vertices.data(), vertices.size() * sizeof(float));
		VertexBufferLayout layout;
		layout.Push<float>(3); // positions
		layout.Push<float>(2); // texCoord
		m_VA->AddBuffer(*m_VB, layout);
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

	void TestHeightMap::OnRender()
	{
		GLCall(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
		GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
		float fovScale = 0.75f;

		m_Shader->Bind();
		glm::mat4 projection = glm::perspective(glm::radians(m_Camera.Zoom), m_cameraController.GetAspectRatio(), 0.1f, 100000.0f);
		glm::mat4 view = m_Camera.GetViewMatrix();
		float fov = glm::radians(m_Camera.Zoom);
		float fovCos = glm::cos(fov * fovScale);
		m_Shader->setFloat("fovCos", fovCos);
		m_Shader->setMat4("view", view);
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
		glDrawArrays(GL_PATCHES, 0, NUM_PATCH_PTS * rez * rez);
		m_Shader->Unbind();

		if (showNormals)
		{
			m_NormalShader->Bind();
			// Set the same uniforms as main shader
			m_NormalShader->setMat4("view", view);
			m_NormalShader->setMat4("projection", projection);
			m_NormalShader->setMat4("model", model);
			m_NormalShader->setVec2("uTexelSize", { 1.0f / m_width, 1.0f / m_height });
			m_NormalShader->setFloat("normalLength", 2.0f); // Adjust length as needed

			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			glDrawArrays(GL_PATCHES, 0, NUM_PATCH_PTS * rez * rez);
			m_NormalShader->Unbind();
		}
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