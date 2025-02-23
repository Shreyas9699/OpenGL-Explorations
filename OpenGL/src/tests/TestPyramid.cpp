#include "TestPyramid.h"
#include "imgui/imgui.h"
#include <iostream>


namespace test
{
	TestPyramid::TestPyramid(GLFWwindow* window)
		: m_Window(window),
		  m_Color{ 1.0f, 0.5f, 0.31f, 1.0f },
		  m_Camera(glm::vec3(-3.0f, 1.0f, -3.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), 45.0f, 0.0f),
		  m_cameraController(m_Window, m_Camera)
	{
		glEnable(GL_DEPTH_TEST);
		glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

		std::vector<float> pyramidVertices = {
			// position(x, y, z) and texture(x, y) coordinates
			-1.0f, 0.0f, -1.0f,		0.0f, 0.0f,	// Base Triangle 1
			-1.0f, 0.0f,  1.0f,		0.0f, 1.0f,
			 1.0f, 0.0f,  1.0f,		1.0f, 1.0f,

			 1.0f, 0.0f,  1.0f,		1.0f, 1.0f,    // Base Triangle 2
			 1.0f, 0.0f, -1.0f,		1.0f, 0.0f,
			-1.0f, 0.0f, -1.0f,		0.0f, 0.0f,

			-1.0f, 0.0f, -1.0f,		0.0f, 0.0f,    // Side 1
			-1.0f, 0.0f,  1.0f,		1.0f, 0.0f,
			 0.0f, 1.5f,  0.0f,		0.5f, 1.0f,

			-1.0f, 0.0f, -1.0f,		0.0f, 0.0f,    // Side 2
			 1.0f, 0.0f, -1.0f,		1.0f, 0.0f,
			 0.0f, 1.5f,  0.0f,		0.5f, 1.0f,

			 1.0f, 0.0f,  1.0f,		0.0f, 0.0f,    // Side 3
			 1.0f, 0.0f, -1.0f,		1.0f, 0.0f,
			 0.0f, 1.5f,  0.0f,		0.5f, 1.0f,

			-1.0f, 0.0f, 1.0f,		0.0f, 0.0f,     // Side 4
			 1.0f, 0.0f, 1.0f,		1.0f, 0.0f,
			 0.0f, 1.5f, 0.0f,		0.5f, 1.0f,
		};

		//std::vector<uint32_t> indicies = {
		//	0, 1, 2,
		//	0, 2, 3,
		//	0, 3, 4,
		//	0, 4, 1,
		//	1, 2, 3,  // left half of base
		//	1, 3, 4
		//};

		m_VA = std::make_unique<VertexArray>();
		m_VB = std::make_unique<VertexBuffer>(pyramidVertices.data(), pyramidVertices.size() * sizeof(float));
		//m_IB = std::make_unique<IndexBuffer>(indicies.data(), indicies.size() * sizeof(unsigned int));

		VertexBufferLayout layout;
		layout.Push<float>(3);
		layout.Push<float>(2);
		m_VA->AddBuffer(*m_VB, layout);
		m_Texture = std::make_unique<Texture>("res/textures/conatiner/Brick.jpg"); // can use Brick2.jpg
		m_Shader = std::make_unique<Shader>("res/shaders/3DtexVS.glsl", "res/shaders/3DtexFS.glsl");
		m_Shader->Bind();
		m_Shader->setInt("u_Texture", 0);
		m_Shader->Unbind();
		m_Renderer = std::make_unique<Renderer>();
	}

	TestPyramid::~TestPyramid()
	{
		m_VA.reset();
		m_VB.reset();
		m_IB.reset();
		m_Texture.reset();
		m_Shader->Unbind();
		m_Shader.reset();
		m_Renderer.reset();
	}

	void test::TestPyramid::OnUpdate(Timestep deltaTime, GLFWwindow* win)
	{
		m_cameraController.Update(deltaTime);
	}

	void TestPyramid::OnRender()
	{
		m_Renderer->Clear();
		m_Texture->Bind();

		glm::mat4 projection = glm::perspective(glm::radians(m_Camera.Zoom), m_cameraController.GetAspectRatio(), 0.1f, 100.0f);
		glm::mat4 view = m_Camera.GetViewMatrix();
		glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 mvp = projection * view * model;

		m_Shader->Bind();
		m_Shader->setMat4("u_MVP", mvp);
		m_Shader->Unbind();
		m_Renderer->Draw(*m_VA, *m_Shader, 18);
		//m_Renderer->Draw(*m_VA, *m_IB, *m_Shader);
	}

	void TestPyramid::OnImGuiRender()
	{
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		ImGui::Text("FPS: %.1f (%.3f ms)", io.Framerate, 1000.0f / io.Framerate);
	}
}