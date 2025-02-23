#pragma once

#include "Test.h"

namespace test 
{
	class TestClearColor : public Test
	{
	private:
		float m_ClearColor[4];

	public:
		TestClearColor();
		~TestClearColor();

		void OnUpdate(Timestep deltaTime, GLFWwindow* m_Window = nullptr) override;
		void OnRender() override;
		void OnImGuiRender() override;
	};
}