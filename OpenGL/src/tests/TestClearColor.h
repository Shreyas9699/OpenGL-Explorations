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

		void OnUpdate(Timestep deltaTime, GLFWwindow* win = nullptr) override;
		void OnRender() override;
		void OnImGuiRender() override;
	};
}