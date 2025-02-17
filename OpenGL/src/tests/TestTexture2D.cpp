#include "TestTexture2D.h"

#include "Renderer.h"
#include "imgui/imgui.h"

namespace test
{
	TestTexture2D::TestTexture2D()
        : m_Projection(glm::ortho(0.0f, 1280.0f, 0.0f, 720.0f, -1.0f, 1.0f)),
          m_View(glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0))), 
          m_TranslationA(200, 200, 0), m_TranslationB(400, 200, 0)
	{

        float positions[] = {
        -50.0f, -50.0f, 0.0f, 0.0f,
         50.0f, -50.0f, 1.0f, 0.0f,
         50.0f,  50.0f, 1.0f, 1.0f,
        -50.0f,  50.0f, 0.0f, 1.0f
        };

        // using index buffer to draw sq
        unsigned int indices[] = {
            0, 1, 2,
            2, 3, 0
        };

        m_VA = std::make_unique<VertexArray>();
        m_VB = std::make_unique<VertexBuffer>(positions, sizeof(positions) * sizeof(float));
        VertexBufferLayout layout;
        layout.Push<float>(2);
        layout.Push<float>(2);
        m_VA->AddBuffer(*m_VB, layout);
        m_IB = std::make_unique<IndexBuffer>(indices, sizeof(indices));
        m_Renderer = std::make_unique<Renderer>();

        m_Shader = std::make_unique<Shader>("res/shaders/TextureShaderVS.shader", "res/shaders/TextureShaderFS.shader");
        m_Shader->Bind();
        m_Shader->setVec4("u_Color", 0.8f, 0.3f, 0.8f, 1.0f);
        m_Texture = std::make_unique<Texture>("res/textures/SSanW2.png");
        m_Shader->setInt("u_Texture", 0);
	}

	TestTexture2D::~TestTexture2D() 
    {
        m_VA.reset();
        m_VB.reset();
        m_IB.reset();
        m_Texture.reset();
        m_Shader->Unbind();
		m_Shader.reset();
		m_Renderer.reset();
    }

	void TestTexture2D::OnUpdate(Timestep deltaTime, GLFWwindow* win)
	{}

	void TestTexture2D::OnRender()
	{
        glm::mat4 model, mvp;
        m_Texture->Bind();

        {
            model = glm::translate(glm::mat4(1.0f), m_TranslationA);
            mvp = m_Projection * m_View * model;
            m_Shader->Bind();
            m_Shader->setMat4("u_MVP", mvp);
            m_Renderer->Draw(*m_VA, *m_IB, *m_Shader);
        }

        {
            model = glm::translate(glm::mat4(1.0f), m_TranslationB);
            mvp = m_Projection * m_View * model;
            m_Shader->Bind();
            m_Shader->setMat4("u_MVP", mvp);
            m_Renderer->Draw(*m_VA, *m_IB, *m_Shader);
        }
	}

	void TestTexture2D::OnImGuiRender()
	{
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        ImGui::Text("FPS: %.1f (%.3f ms)", io.Framerate, 1000.0f / io.Framerate);
        ImGui::SliderFloat("Translation Ax", &m_TranslationA.x, 0.0f,  1280.0f);
        ImGui::SliderFloat("Translation Ay", &m_TranslationA.y, 0.0f, 720.0f);
        ImGui::SliderFloat("Translation Bx", &m_TranslationB.x, 0.0f, 1280.0f);
        ImGui::SliderFloat("Translation By", &m_TranslationB.y, 0.0f, 720.0f);
	}
}