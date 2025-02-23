#include "TestUVSphere.h"
#include "imgui/imgui.h"

namespace test
{
	TestUVSphere::TestUVSphere(GLFWwindow* window)
		: m_Color{ 0.2f, 0.3f, 0.8f, 1.0f },
		  m_Position { 0.0f, 0.0f, 0.0f },
		  m_Camera(glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f)),
		  m_cameraController(window, m_Camera)
	{   
		m_Sphere = std::make_unique<UVSphere>(m_radius, m_segment, m_rings);

		m_VA = std::make_unique<VertexArray>();
		m_VB = std::make_unique<VertexBuffer>(m_Sphere->GetVertex().data(), m_Sphere->GetVertex().size() * sizeof(float));
		m_IB = std::make_unique<IndexBuffer>(m_Sphere->GetIndex().data(), m_Sphere->GetIndex().size());

		VertexBufferLayout layout;
		layout.Push<float>(3); // positions
		layout.Push<float>(3); // normals
		m_VA->AddBuffer(*m_VB, layout);

		m_Shader = std::make_unique<Shader>("res/shaders/LightSphere/lightSphereVS.shader", 
			"res/shaders/LightSphere/lightSphereFS.shader");

		m_Renderer = std::make_unique<Renderer>();
	}

	TestUVSphere::~TestUVSphere()
	{
		m_VA.reset();
		m_VB.reset();
		m_IB.reset();
		m_Shader->Unbind();
		m_Shader.reset();
		m_Renderer.reset();
	}

	void TestUVSphere::OnUpdate(Timestep deltaTime, GLFWwindow* m_Window)
	{
		m_cameraController.Update(deltaTime);
		if (m_radius != m_prevRadius || m_segment != m_prevSegment || m_rings != m_prevRings)
		{
			m_prevRadius = m_radius;
			m_prevSegment = m_segment;
			m_prevRings = m_rings;

			m_Sphere->ReGenerate(m_radius, m_segment, m_rings);

			m_VA.reset();
			m_VB.reset();
			m_IB.reset();

			m_VA = std::make_unique<VertexArray>();
			m_VB = std::make_unique<VertexBuffer>(m_Sphere->GetVertex().data(), m_Sphere->GetVertex().size() * sizeof(float));
			m_IB = std::make_unique<IndexBuffer>(m_Sphere->GetIndex().data(), m_Sphere->GetIndex().size());

			VertexBufferLayout layout;
			layout.Push<float>(3);
			layout.Push<float>(3);
			m_VA->AddBuffer(*m_VB, layout);
		}
	}

	void TestUVSphere::OnRender()
	{
		glm::mat4 projection = glm::perspective(glm::radians(m_Camera.Zoom), m_cameraController.GetAspectRatio(), 0.1f, 100.0f);
		glm::mat4 view = m_Camera.GetViewMatrix();
		m_Shader->Bind();
		m_Shader->setMat4("view", view);
		m_Shader->setMat4("projection", projection);
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, m_Position);
		m_Shader->setMat4("model", model);
		m_Shader->setVec3("lightColor", m_Color[0], m_Color[1], m_Color[2]);
		m_Shader->Unbind();
		if (m_IsFilled)
		{
			m_Renderer->Draw(*m_VA, *m_IB, *m_Shader);
		}
		else
		{
			m_Renderer->DrawLines(*m_VA, *m_IB, *m_Shader);
		}
	}

	void TestUVSphere::OnImGuiRender()
	{
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		ImGui::Text("FPS: %.1f (%.3f ms)", io.Framerate, 1000.0f / io.Framerate);

		ImGui::ColorEdit4("Change Color", m_Color);
		ImGui::SliderFloat("Radius", &m_radius, 1.0f, 10.0f);
		ImGui::SliderScalar("Segments", ImGuiDataType_U32, &m_segment, &minSegments, &maxSegments, "%u");
		ImGui::SliderScalar("Rings", ImGuiDataType_U32, &m_rings, &minRings, &maxRings, "%u");
		ImGui::Checkbox("Fill the Sphere", &m_IsFilled);
	}
}