#pragma once

#include "Test.h"
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "Texture.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include <memory>

namespace test
{
	class TestTexture2D : public Test
	{
	private:
		glm::vec3 m_TranslationA, m_TranslationB;
		glm::mat4 m_Projection, m_View;
		std::unique_ptr<VertexArray> m_VA;
		std::unique_ptr<VertexBuffer> m_VB;
		std::unique_ptr<IndexBuffer> m_IB;
		std::unique_ptr<Shader> m_Shader;
		std::unique_ptr<Texture> m_Texture;
		std::unique_ptr<Renderer> m_Renderer;

	public:
		TestTexture2D();
		~TestTexture2D();

		void OnUpdate(Timestep deltaTime, GLFWwindow* win = nullptr) override;
		void OnRender() override;
		void OnImGuiRender() override;
	};
}