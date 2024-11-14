#pragma once

#include "Test.h"
#include "VertexArray.h"
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "IndexBuffer.h"
#include "Shader.h"
#include "Renderer.h"

#include <memory>

namespace test
{
	class TestTriangle : public Test
	{
	private:
		float m_Color[4];
		std::unique_ptr<VertexArray> m_VA;
		std::unique_ptr<VertexBuffer> m_VB;
		std::unique_ptr<IndexBuffer> m_IB;
		std::unique_ptr<Shader> m_Shader;
	public:
		TestTriangle();
		~TestTriangle();

		void OnUpdate(Timestep deltaTime) override;
		void OnRender() override;
		void OnImGuiRender() override;
	};
}