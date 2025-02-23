#include "TestLoadModels.h"
#include "imgui/imgui.h"
#include <iostream>

namespace test
{
	TestLoadModels::TestLoadModels(Window* window)
		: m_Window(window),
		  m_Camera(glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f)),
		  m_cameraController(m_Window->GetWindow(), m_Camera)
	{
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_MULTISAMPLE);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		m_Window->setCustomKeyCallback([this](int key, int scancode, int action, int mods)
			{
				this->handleKeyPress(key, scancode, action, mods);
			});
		glfwSetInputMode(m_Window->GetWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

		m_Shader = std::make_unique<Shader>("res/shaders/modelVS.glsl", "res/shaders/modelFS.glsl");

		m_Shader->use();
		m_Shader->setVec3("lightPos", 10.0f, 10.0f, 10.0f);
		m_Shader->setVec3("lightColor", 1.0f, 1.0f, 1.0f);
		m_Shader->Unbind();

		currModelPath = "res/objects/model_testing.obj";
		selectedModel = currModelPath;
		model_testing = std::make_unique<Model>(currModelPath.c_str());
	}

	TestLoadModels::~TestLoadModels()
	{
		model_testing.reset();
		m_Shader->Unbind();
		m_Shader.reset();

		m_Window->setCustomKeyCallback(nullptr);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	void TestLoadModels::handleKeyPress(int key, int scancode, int action, int mods)
	{
		if (key == GLFW_KEY_T && action == GLFW_PRESS)
		{
			isWireFrame = !isWireFrame;
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
				m_Camera.Position = glm::vec3(0.0f, 0.0f, 5.0f);
				m_Camera.Up = glm::vec3(0.0f, 1.0f, 0.0f);
				m_Camera.Front = glm::vec3(0.0f, 0.0f, 0.0f);
			}
			else
			{
				glfwSetInputMode(m_Window->GetWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
				m_Camera.Position = glm::vec3(0.0f, 0.0f, 5.0f);
				m_Camera.Up = glm::vec3(0.0f, 1.0f, 0.0f);
				m_Camera.Front = glm::vec3(0.0f, 0.0f, 0.0f);
			}
		}

	}

	void TestLoadModels::OnUpdate(Timestep deltaTime, GLFWwindow* win)
	{
		m_cameraController.Update(deltaTime);

		if (selectedModel != currModelPath)
		{
			currModelPath = selectedModel;
			model_testing.reset();
			model_testing = std::make_unique<Model>(currModelPath.c_str());
		}
	}

	void TestLoadModels::OnRender()
	{
		GLCall(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
		GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
		m_Shader->use();
		glm::mat4 projection = glm::perspective(glm::radians(m_Camera.Zoom), m_cameraController.GetAspectRatio(), 0.1f, 100.0f);
		m_Shader->setMat4("model", glm::mat4(1.0f));
		m_Shader->setMat4("view", m_Camera.GetViewMatrix());
		m_Shader->setVec3("viewPos", m_Camera.Position);
		m_Shader->setMat4("projection", projection);
		if (isWireFrame)
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}
		else
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
		for (unsigned int i = 0; i < model_testing->num_meshes; ++i)
		{
			m_Shader->setVec3("diffuseColor", model_testing->mesh_list[i].diffuseColor.r, model_testing->mesh_list[i].diffuseColor.g,
		        model_testing->mesh_list[i].diffuseColor.b);
			m_Shader->setBool("isTex", model_testing->mesh_list[i].hasTexture);
		    
		    if (model_testing->mesh_list[i].hasTexture)
		    {
		        glActiveTexture(GL_TEXTURE0);
		        glBindTexture(GL_TEXTURE_2D, model_testing->mesh_list[i].tex_handle); // Bind texture for the current mesh
		    }
		
		    glBindVertexArray(model_testing->mesh_list[i].VAO);
		    glDrawElements(GL_TRIANGLES, (GLsizei)model_testing->mesh_list[i].vert_indices.size(), GL_UNSIGNED_INT, 0);
		    glBindVertexArray(0);
		}
		m_Shader->Unbind();
	}

	void TestLoadModels::ShowFileExplorer()
	{
		static std::string rootPath = "res/objects";
		// Display header
		ImGui::Separator();
		ImGui::Text("OBJ files under: %s", rootPath.c_str());
		ImGui::Separator();

		try
		{
			// Use recursive_directory_iterator to traverse all subfolders
			for (const auto& entry : std::filesystem::recursive_directory_iterator(rootPath))
			{
				// Only process files (not directories) with .obj extension
				if (!entry.is_directory() && entry.path().extension() == ".obj")
				{
					std::string displayPath = entry.path().string(); 
					std::replace(displayPath.begin(), displayPath.end(), '\\', '/');
					bool isSelected = (displayPath == selectedModel);

					if (ImGui::Selectable(displayPath.c_str(), isSelected))
					{
						selectedModel = displayPath;
						std::cout << "Selected file: " << selectedModel << std::endl;
					}
				}
			}
		}
		catch (const std::filesystem::filesystem_error& e)
		{
			ImGui::TextColored(ImVec4(1, 0, 0, 1), "Error accessing directory: %s", e.what());
		}

		ImGui::Separator();
	}

	void TestLoadModels::OnImGuiRender()
	{
		ImVec4 enabledColor = ImVec4(0.0f, 1.0f, 0.0f, 1.0f);  // Green
		ImVec4 disabledColor = ImVec4(1.0f, 0.0f, 0.0f, 1.0f); // Red
		ImVec4 color;
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		ImGui::Text("FPS: %.1f (%.3f ms)", io.Framerate, 1000.0f / io.Framerate);

		ImGui::Text("Current Model : %s",
			(currModelPath.substr(
				(currModelPath.find_last_of("/\\") != std::string::npos) ?
				currModelPath.find_last_of("/\\") + 1
				: 0)).c_str());
		ImGui::SameLine();

		if (ImGui::Button("Select Object"))
		{
			currentPath = "res/objects";
			ImGui::SetNextWindowViewport(ImGui::GetMainViewport()->ID);
			ImGui::OpenPopup("File Explorer");
		}
		
		/*if (showFileExplorer)
		{
			ShowFileExplorer();
		}*/

		// Begin the popup
		if (ImGui::BeginPopupModal("File Explorer", nullptr,
			ImGuiWindowFlags_AlwaysAutoResize |
			ImGuiWindowFlags_NoDocking |
			ImGuiWindowFlags_NoSavedSettings))
		{
			ShowFileExplorer();  // Draw the file explorer contents

			// Optional: a close button for the popup
			if (ImGui::Button("Close"))
			{
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}

		ImGui::Text("Press below keys to enable/disable:\n");

		color = cusorEnable ? enabledColor : disabledColor;
		ImGui::Text("`Q` -> Cursor (Mouse pointer) ->");
		ImGui::SameLine();
		ImGui::TextColored(color, " [%s]\n", (cusorEnable) ? "ENABLED" : "DISABLED");

		color = isWireFrame ? enabledColor : disabledColor;
		ImGui::Text("`T` -> WireFrame View  ->");
		ImGui::SameLine();
		ImGui::TextColored(color, " [%s]\n", (isWireFrame) ? "ENABLED" : "DISABLED");

		/*color = dynamicTess ? enabledColor : disabledColor;
		ImGui::Text("`E` -> Dynamic Tessellation ->");
		ImGui::SameLine();
		ImGui::TextColored(color, " [%s]\n", (dynamicTess) ? "ENABLED" : "DISABLED");

		color = renderPointsOnly ? enabledColor : disabledColor;
		ImGui::Text("`P` -> Tessellated Points ->");
		ImGui::SameLine();
		ImGui::TextColored(color, " [%s]\n", (renderPointsOnly) ? "ENABLED" : "DISABLED");

		color = showNormals ? enabledColor : disabledColor;
		ImGui::Text("`N` -> Normal ->");
		ImGui::SameLine();
		ImGui::TextColored(color, " [%s]\n", (showNormals) ? "ENABLED" : "DISABLED");*/
	}
}