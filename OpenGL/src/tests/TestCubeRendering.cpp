#include "TestCubeRendering.h"
#include "imgui/imgui.h"
#include <iostream>

namespace test
{
	TestCubeRendering::TestCubeRendering(GLFWwindow* window)
		: ObjectColor{ 1.0f, 0.5f, 0.31f, 1.0f }, 
		  m_Camera(glm::vec3(-1.0f, 1.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), 45.0f, 0.0f),
		  m_cameraController(window, m_Camera)
	{
		float vertices[] = {
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
		};

		m_cube.VAO = std::make_unique<VertexArray>();
		m_cube.VBO = std::make_unique<VertexBuffer>(vertices, sizeof(vertices));
		VertexBufferLayout layout1;
		layout1.Push<float>(3);
		layout1.Push<float>(3);
		m_cube.VAO->AddBuffer(*m_cube.VBO, layout1);

		m_Sphere = std::make_unique<UVSphere>(1.0f, 64, 32);
		m_sphere.VAO = std::make_unique<VertexArray>();
		m_sphere.VBO = std::make_unique<VertexBuffer>(m_Sphere->GetVertex().data(), m_Sphere->GetVertex().size() * sizeof(float));
		m_sphere.IBO = std::make_unique<IndexBuffer>(m_Sphere->GetIndex().data(), m_Sphere->GetIndex().size());

		VertexBufferLayout layout2;
		layout2.Push<float>(3); // positions
		layout2.Push<float>(3); // normals
		m_sphere.VAO->AddBuffer(*m_sphere.VBO, layout2);

		m_ShaderCube = std::make_unique<Shader>("res/shaders/CubrWithLightVS.shader", "res/shaders/CubrWithLightFS.shader");
		m_ShaderLight = std::make_unique<Shader>("res/shaders/LightSphere/lightSphereVS.shader",
			"res/shaders/LightSphere/lightSphereFS.shader");
		m_Renderer = std::make_unique<Renderer>();

		for (const auto& name : material_types)
		{
			material_names.push_back(name.c_str());
		}


		lightProperties.baseColor = { 1.0f, 1.0f, 1.0f };
		lightProperties.ambientStrength = 1.0f;
		lightProperties.diffuseStrength = 1.0f;
		lightProperties.specularStrength = 1.0f;
	}

	TestCubeRendering::~TestCubeRendering()
	{
		m_Sphere.reset();
		m_Renderer.reset();
		m_ShaderCube->Unbind();
		m_ShaderCube.reset();
		m_ShaderLight->Unbind();
		m_ShaderLight.reset();
	}

	void TestCubeRendering::OnUpdate(Timestep deltaTime, GLFWwindow* win)
	{
		m_cameraController.Update(deltaTime);

		// update light color properties
		lightProperties.ambient = lightProperties.baseColor * lightProperties.ambientStrength;
		lightProperties.diffuse = lightProperties.baseColor * lightProperties.diffuseStrength;
		lightProperties.specular = lightProperties.baseColor * lightProperties.specularStrength;
	}

	void TestCubeRendering::OnRender()
	{
		GLCall(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
		GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT););

		m_ShaderCube->Bind();
		m_ShaderCube->setVec3("viewPos", m_Camera.Position);

		// light properties
		m_ShaderCube->setVec3("light.position", lightPosition);
		m_ShaderCube->setVec3("light.ambient", lightProperties.ambient);
		m_ShaderCube->setVec3("light.diffuse", lightProperties.diffuse);
		m_ShaderCube->setVec3("light.specular", lightProperties.specular);

		// material properties
		m_ShaderCube->setVec3("material.ambient", mat_prop.mat_ambient[0], mat_prop.mat_ambient[1], mat_prop.mat_ambient[2]);
		m_ShaderCube->setVec3("material.diffuse", mat_prop.mat_diffuse[0], mat_prop.mat_diffuse[1], mat_prop.mat_diffuse[2]);
		m_ShaderCube->setVec3("material.specular", mat_prop.mat_specular[0], mat_prop.mat_specular[1], mat_prop.mat_specular[2]);
		m_ShaderCube->setFloat("material.shininess", mat_prop.mat_shininess);

		// view/projection transformations
		glm::mat4 projection = glm::perspective(glm::radians(m_Camera.Zoom), 900.0f / 900.0f, 0.1f, 100.0f);
		glm::mat4 view = m_Camera.GetViewMatrix();
		m_ShaderCube->setMat4("view", view);
		m_ShaderCube->setMat4("projection", projection);

		glm::mat4 model = glm::mat4(1.0f);
		m_ShaderCube->setMat4("model", model);
		m_ShaderCube->Unbind();
		m_Renderer->Draw(*m_cube.VAO, *m_ShaderCube, 36);

		// Draw the second cube
		m_ShaderLight->Bind();
		m_ShaderLight->setMat4("view", view);
		m_ShaderLight->setMat4("projection", projection);
		model = glm::mat4(1.0f);
		model = glm::translate(model, lightPosition);
		model = glm::scale(model, glm::vec3(0.2f));
		m_ShaderLight->setMat4("model", model);
		m_ShaderLight->setVec3("lightColor", lightProperties.baseColor);
		m_ShaderLight->Unbind();
		m_Renderer->Draw(*m_sphere.VAO, *m_sphere.IBO ,*m_ShaderLight);
	}

	void TestCubeRendering::OnImGuiRender()
	{
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		ImGui::Text("FPS: %.1f (%.3f ms)", io.Framerate, 1000.0f / io.Framerate);

		//ImGui::ColorEdit4("Change Object Color", ObjectColor);

		ImGui::Text("Select Material:");
		if (ImGui::Combo("Material Type", &current_material_index, material_names.data(), material_names.size()))
		{
			mat_prop = getMaterialProperties(static_cast<MaterialType>(current_material_index));
		}

		// Slider for Shininess
		ImGui::Text("Shininess:");
		ImGui::SliderFloat("Value", &mat_prop.mat_shininess, 1.0f, 256.0f); // Adjust max shininess as needed

		ImGui::Text("Light Properties");
		ImGui::ColorEdit3("Base Color", (float*)&lightProperties.baseColor);
		ImGui::Text("Ambient Strength:");
		ImGui::SliderFloat("AS", &lightProperties.ambientStrength, 0.0f, 1.0f);
		ImGui::Text("Diffuse Strength:");
		ImGui::SliderFloat("DS", &lightProperties.diffuseStrength, 0.0f, 1.0f);
		ImGui::Text("Specular Strength:");
		ImGui::SliderFloat("SS", &lightProperties.specularStrength, 0.0f, 1.0f);
	}
}
