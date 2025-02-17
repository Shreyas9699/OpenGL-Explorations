#include "TestFBMPlane.h"
#include "imgui/imgui.h"
#include <iostream>

namespace test
{
	TestFBMPlane::TestFBMPlane(Window* win)
		: m_window(win),
		  m_planeColor{ 0.2f, 0.6f, 0.8f, 1.0f },
		  lightColor{ 1.0f, 1.0f, 1.0f , 1.0f },
		  m_Camera(glm::vec3(-1.0f, 1.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), 45.0f, 0.0f),
		  m_cameraController(win->getWindow(), m_Camera)
	{
		//glfwSetInputMode(m_window->getWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		m_window->setCustomKeyCallback([this](int key, int scancode, int action, int mods)
			{
				this->handleKeyPress(key, scancode, action, mods);
			});

		m_GridSizePrev = m_GridSize;
		m_PlaneSizePrev = m_PlaneSize;
		// setting plane attributes
		GeneratePlane();

		// setting sphere(light bulb) attributes
		m_Sphere = std::make_unique<UVSphere>(0.5f, 64, 32);
		m_sphere.VAO = std::make_unique<VertexArray>();
		m_sphere.VBO = std::make_unique<VertexBuffer>(m_Sphere->GetVertex().data(), m_Sphere->GetVertex().size() * sizeof(float));
		m_sphere.IBO = std::make_unique<IndexBuffer>(m_Sphere->GetIndex().data(), m_Sphere->GetIndex().size());
		VertexBufferLayout layout2;
		layout2.Push<float>(3);
		layout2.Push<float>(3);
		m_sphere.VAO->AddBuffer(*m_sphere.VBO, layout2);

		m_ShaderPlane = std::make_unique<Shader>("res/shaders/FBM/VertexFBMShader.glsl", "res/shaders/FBM/FragShaderFBM.glsl");
		m_ShaderLightSrc = std::make_unique<Shader>("res/shaders/LightSphere/lightSphereVS.shader",
			"res/shaders/LightSphere/lightSphereFS.shader");
		m_Renderer = std::make_unique<Renderer>();

		lightPos = glm::vec3(0.0f, 1.0f, 0.0f);
	}

	TestFBMPlane::~TestFBMPlane()
	{
		m_plane.VAO.reset();
		m_plane.VBO.reset();
		m_plane.IBO.reset();
		m_Sphere.reset();
		m_ShaderPlane->Unbind();
		m_ShaderPlane.reset();
		m_ShaderLightSrc->Unbind();
		m_ShaderLightSrc.reset();
		m_Renderer.reset();
	}

	void TestFBMPlane::GeneratePlane()
	{
		// generating plane vertices
		m_Vertices.clear();
		m_Indices.clear();

		m_plane.VAO.reset();
		m_plane.VBO.reset();
		m_plane.IBO.reset();

		int halfGridSize = m_GridSize / 2;

		m_Vertices.reserve(static_cast<size_t>((m_GridSize + 1) * (m_GridSize + 1) * 6));
		for (int z = -halfGridSize; z <= halfGridSize; z++)
		{
			for (int x = -halfGridSize; x <= halfGridSize; x++)
			{
				float posX = ((float)x / halfGridSize) * (m_PlaneSize / 2);
				float posZ = ((float)z / halfGridSize) * (m_PlaneSize / 2);

				m_Vertices.push_back(posX);
				m_Vertices.push_back(0.0f);
				m_Vertices.push_back(posZ);
				m_Vertices.push_back(0.0f);
				m_Vertices.push_back(1.0f);
				m_Vertices.push_back(0.0f);
			}
		}

		m_Indices.reserve(static_cast<size_t>(m_GridSize * m_GridSize * 6));
		for (int z = 0; z < m_GridSize; z++)
		{
			for (int x = 0; x < m_GridSize; x++)
			{
				uint32_t topLeft = static_cast<uint32_t>(z * (m_GridSize + 1) + x);
				uint32_t topRight = topLeft + 1;
				uint32_t bottomLeft = static_cast<uint32_t>((z + 1) * (m_GridSize + 1) + x);
				uint32_t bottomRight = bottomLeft + 1;

				// First triangle
				m_Indices.push_back(topLeft);
				m_Indices.push_back(bottomLeft);
				m_Indices.push_back(topRight);

				// Second triangle
				m_Indices.push_back(topRight);
				m_Indices.push_back(bottomLeft);
				m_Indices.push_back(bottomRight);
			}
		}

		// setting plane attributes
		m_plane.VAO = std::make_unique<VertexArray>();
		m_plane.VBO = std::make_unique<VertexBuffer>(m_Vertices.data(), m_Vertices.size() * sizeof(float));
		m_plane.IBO = std::make_unique<IndexBuffer>(m_Indices.data(), m_Indices.size());
		VertexBufferLayout layout1;
		layout1.Push<float>(3);
		layout1.Push<float>(3);
		m_plane.VAO->AddBuffer(*m_plane.VBO, layout1);
	}

	void TestFBMPlane::handleKeyPress(int key, int scancode, int action, int mods)
	{
		if (key == GLFW_KEY_R && action == GLFW_PRESS)
		{
			// this flag is used to rotate the plane
			isRotating = !isRotating; 
		}
		if (key == GLFW_KEY_T && action == GLFW_PRESS)
		{
			// this flag is used to render the plane in wireframe mode
			isWireFrame = !isWireFrame;
		}
		if (key == GLFW_KEY_M && action == GLFW_PRESS)
		{
			// this flag is used to handle the FBM plane transformation, ie to have a const or move with time
			isMoving = !isMoving;
		}
		if (key == GLFW_KEY_Q && action == GLFW_PRESS)
		{
			cusorEnable = !cusorEnable;
			if (cusorEnable)
			{
				glfwSetInputMode(m_window->getWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			}
			else
			{
				glfwSetInputMode(m_window->getWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			}
		}
	}

	void TestFBMPlane::OnUpdate(Timestep deltaTime, GLFWwindow* win)
	{
		if (m_GridSizePrev != m_GridSize || m_PlaneSizePrev != m_PlaneSize)
		{
			m_GridSizePrev = m_GridSize;
			m_PlaneSizePrev = m_PlaneSize;
			GeneratePlane();
		}
		m_cameraController.Update(deltaTime);

		if (isMoving)
		{
			lastTtime = float(glfwGetTime());;

		}

		if (isRotating)
		{
			horizontalrotationAngle += rotationSpeed * deltaTime; // Rotate FBM tterrain clockwise
		}
	}

	void TestFBMPlane::OnRender()
	{
		GLCall(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
		GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

		m_ShaderPlane->Bind();
		
		m_ShaderPlane->setFloat("time", lastTtime);
		// view/projection transformations
		glm::mat4 projection = glm::perspective(glm::radians(m_Camera.Zoom), m_cameraController.GetAspectRatio(), 0.1f, 100.0f);
		glm::mat4 view = m_Camera.GetViewMatrix();
		m_ShaderPlane->setMat4("view", view);
		m_ShaderPlane->setMat4("projection", projection);
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(horizontalrotationAngle), glm::vec3(0.0f, 1.0f, 0.0f));
		m_ShaderPlane->setMat4("model", model);
		m_ShaderPlane->setVec3("lightPos", lightPos);
		m_ShaderPlane->setVec3("lightColor", lightColor[0], lightColor[1], lightColor[2]);
		m_ShaderPlane->setVec3("objectColor", m_planeColor[0], m_planeColor[1], m_planeColor[2]);
		m_ShaderPlane->setVec3("viewPos", m_Camera.Position);
		m_ShaderPlane->Unbind();
		if (isWireFrame)
		{
			GLCall(glPolygonMode(GL_FRONT_AND_BACK, GL_LINE));
		}
		else
		{
			GLCall(glPolygonMode(GL_FRONT_AND_BACK, GL_FILL));
		}
		m_Renderer->Draw(*m_plane.VAO, *m_plane.IBO, *m_ShaderPlane);

		// Draw the second cube
		m_ShaderLightSrc->Bind();
		m_ShaderLightSrc->setMat4("view", view);
		m_ShaderLightSrc->setMat4("projection", projection);
		model = glm::mat4(1.0f);
		model = glm::translate(model, lightPos);
		model = glm::scale(model, glm::vec3(0.2f));
		m_ShaderLightSrc->setMat4("model", model);
		m_ShaderLightSrc->setVec3("lightColor", lightColor[0], lightColor[1], lightColor[2]);
		m_ShaderLightSrc->Unbind();
		m_Renderer->Draw(*m_sphere.VAO, *m_sphere.IBO ,*m_ShaderLightSrc);
	}

	void TestFBMPlane::OnImGuiRender()
	{
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		ImGui::Text("FPS: %.1f (%.3f ms)", io.Framerate, 1000.0f / io.Framerate);
		ImGui::ColorEdit4("Change Object Color", m_planeColor);
		ImGui::ColorEdit4("Light Color", lightColor);
		SliderIntEvent("Grid Size", &m_GridSize, 2, 50, 2);
		ImGui::SliderFloat("Plane Size", &m_PlaneSize, 1.0f, 50.0f);
		ImGui::Text("Press below keys to enable/disable:\n");
		ImGui::Text("`R` -> Plane Rotation\n");
		ImGui::Text("`T` -> Rendering Plane in Wireframe\n");
		ImGui::Text("`M` -> Plane Movement\n");
		//ImGui::Text("Press `Q` to enable/disable Cursor (Mouse pointer)\n");
	}

	void TestFBMPlane::SliderIntEvent(const char* label, int* value, int min, int max, int step)
	{
		// Custom Slider Function with step to always get even values
		ImGui::SliderInt(label, value, min, max);
		if (*value % 2 != 0)
		{
			*value += 1; // Adjust to the next even number
		}
	}
}
