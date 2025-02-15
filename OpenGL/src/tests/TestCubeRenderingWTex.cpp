#include "TestCubeRenderingWTex.h"
#include "imgui/imgui.h"
#include <iostream>

namespace test
{
	TestCubeWithTex::TestCubeWithTex(GLFWwindow* window)
		: ObjectColor{ 1.0f, 0.5f, 0.31f, 1.0f }, 
		  m_Camera(glm::vec3(0.5f, 1.0f, 6.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), -90.0f, 0.0f),
		  m_cameraController(window, m_Camera)
	{
		float vertices[] = {
			// positions          // normals           // texture coords
			-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
			 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
			 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
			 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
			-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
			-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

			-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
			 0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
			 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
			 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
			-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
			-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

			-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
			-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
			-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
			-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
			-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
			-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

			 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
			 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
			 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
			 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
			 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
			 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

			-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
			 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
			 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
			 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
			-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
			-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

			-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
			 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
			 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
			 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
			-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
			-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
		};

		cubePositions = {
			glm::vec3(0.0f, 0.0f, 0.0f),
			glm::vec3(-1.5f, -2.2f, -2.5f),
			glm::vec3(2.4f, -0.4f, -3.5f),
			glm::vec3(1.3f, -2.0f, -2.5f),
			glm::vec3(1.5f, 2.0f, -2.5f),
			glm::vec3(1.5f, 0.2f, -1.5f),
			glm::vec3(-1.3f, 1.0f, -1.5f)
		};

		m_VA = std::make_unique<VertexArray>();
		m_VB = std::make_unique<VertexBuffer>(vertices, sizeof(vertices));
		VertexBufferLayout layout;
		layout.Push<float>(3);
		layout.Push<float>(3);
		layout.Push<float>(2);
		m_VA->AddBuffer(*m_VB, layout);

		m_ShaderCube = std::make_unique<Shader>("res/shaders/CubrWithTexVS.shader", "res/shaders/CubrWithTexFS.shader");
		m_ShaderLight = std::make_unique<Shader>("res/shaders/lightCubeVS.shader", "res/shaders/lightCubeFS.shader");

		m_TextureDiffuse = std::make_unique<Texture>("res/textures/container2.png");
		m_TextureSpecular = std::make_unique<Texture>("res/textures/container2_specular.png");
		m_EmissionMap = std::make_unique<Texture>("res/textures/container2_emissionMap.png");

		m_Renderer = std::make_unique<Renderer>();

		lightProperties.baseColor = { 1.0f, 1.0f, 1.0f };
		lightProperties.ambientStrength = 0.5f;
		lightProperties.diffuseStrength = 1.0f;
		lightProperties.specularStrength = 1.0f;
		lightProperties.constant = 1.0f;
		lightProperties.linear = 0.09f;
		lightProperties.quadratic = 0.032f;


		m_ShaderCube->Bind();
		m_ShaderCube->setInt("material.diffuse", 0);
		m_ShaderCube->setInt("material.specular", 1);
		m_ShaderCube->setInt("material.emission", 2);
		m_ShaderCube->Unbind();
	}

	TestCubeWithTex::~TestCubeWithTex()
	{
		m_TextureDiffuse.reset();
		m_TextureSpecular.reset();
		m_EmissionMap.reset();
		m_ShaderCube->Unbind();
		m_ShaderLight->Unbind();
		m_Renderer.reset();
	}

	void TestCubeWithTex::OnUpdate(Timestep deltaTime, GLFWwindow* win)
	{
		if (!win)
		{
			std::cout << "Window is not null" << std::endl;
		}
		
		// Update camera settings based on camera movement
		m_cameraController.Update(deltaTime);

		// reset Attenuation values if the m_Attenuation is unchcked
		if (!m_Attenuation)
		{  // Reset values if checkbox is unchecked
			lightProperties.constant = 1.0f;
			lightProperties.linear = 0.09f;
			lightProperties.quadratic = 0.032f;
		}

		// update light color properties
		lightProperties.ambient = lightProperties.baseColor * lightProperties.ambientStrength;
		lightProperties.diffuse = lightProperties.baseColor * lightProperties.diffuseStrength;
		lightProperties.specular = lightProperties.baseColor * lightProperties.specularStrength;
	}

	void TestCubeWithTex::OnRender()
	{
		m_ShaderCube->Bind();
		m_ShaderCube->setVec3("viewPos", m_Camera.Position);

		// light properties
		// move light is circle (xy plane)
		/*lightPosition.x = cos(glfwGetTime()) * 1.0f;
		lightPosition.z = sin(glfwGetTime()) * 1.0f;*/
		m_ShaderCube->setVec3("light.position", lightPosition);
		m_ShaderCube->setVec3("light.ambient", lightProperties.ambient);
		m_ShaderCube->setVec3("light.diffuse", lightProperties.diffuse);
		m_ShaderCube->setVec3("light.specular", lightProperties.specular);

		m_ShaderCube->setBool("isAttenuation", m_Attenuation);
		m_ShaderCube->setFloat("light.constant", lightProperties.constant);
		m_ShaderCube->setFloat("light.linear", lightProperties.linear);
		m_ShaderCube->setFloat("light.quadratic", lightProperties.quadratic);

		// material properties
		m_ShaderCube->setFloat("material.shininess", 64.0f);

		// view/projection transformations
		glm::mat4 projection = glm::perspective(glm::radians(m_Camera.Zoom), m_cameraController.GetAspectRatio(), 0.1f, 100.0f);
		glm::mat4 view = m_Camera.GetViewMatrix();
		m_ShaderCube->setMat4("view", view);
		m_ShaderCube->setMat4("projection", projection);
		m_TextureDiffuse->Bind(0);
		m_EmissionMap->Bind(2);
		m_TextureSpecular->Bind(1);
		
		for (unsigned int i = 0; i < cubePositions.size(); i++)
		{
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, cubePositions[i]);
			float angle = 20.0f * i; // Rotate each cube differently
			model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
			m_ShaderCube->setMat4("model", model);
			m_ShaderCube->Unbind();
			m_Renderer->Draw(*m_VA, *m_ShaderCube, 36);
		}

		// Draw the second cube
		m_ShaderLight->Bind();
		m_ShaderLight->setVec3("lightColor", lightProperties.baseColor);
		m_ShaderLight->setMat4("view", view);
		m_ShaderLight->setMat4("projection", projection);
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, lightPosition);
		model = glm::scale(model, glm::vec3(0.2f));
		m_ShaderLight->setMat4("model", model);
		m_ShaderLight->Unbind();
		m_Renderer->Draw(*m_VA, *m_ShaderLight, 36);
	}

	void TestCubeWithTex::OnImGuiRender()
	{
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		ImGui::Text("FPS: %.1f (%.3f ms)", io.Framerate, 1000.0f / io.Framerate);

		//ImGui::ColorEdit4("Change Object Color", ObjectColor);
		ImGui::Text("Light Properties");
		ImGui::ColorEdit3("Base Color", (float*)&lightProperties.baseColor);
		ImGui::Text("Ambient Strength:");
		ImGui::SliderFloat("AS", &lightProperties.ambientStrength, 0.0f, 1.0f);
		ImGui::Text("Diffuse Strength:");
		ImGui::SliderFloat("DS", &lightProperties.diffuseStrength, 0.0f, 1.0f);
		ImGui::Text("Specular Strength:");
		ImGui::SliderFloat("SS", &lightProperties.specularStrength, 0.0f, 1.0f);

		ImGui::Checkbox("Enable Attenuation", &m_Attenuation);
		ImGui::BeginDisabled(!m_Attenuation);
		ImGui::SliderFloat("Constant", &lightProperties.constant, 0.000f, 1.000f);
		ImGui::SliderFloat("Linear", &lightProperties.linear, 0.00f, 1.000f);
		ImGui::SliderFloat("Quadratic", &lightProperties.quadratic, 0.000f, 1.000f);
		ImGui::EndDisabled();

	}
}
