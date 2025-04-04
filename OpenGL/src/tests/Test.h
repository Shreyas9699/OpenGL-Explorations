#pragma once

#include "Timestep.h"
#include "VertexArray.h"
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "IndexBuffer.h"

#include <functional>
#include <vector>
#include <string>
#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

namespace test 
{
	class Test
	{
	public:
		Test() {}
		virtual ~Test() {}
		
		virtual void OnUpdate(Timestep deltaTime, GLFWwindow* m_Window = nullptr) {}
		virtual void OnRender() {}
		virtual void OnImGuiRender() {}
	};

	class TestMenu: public Test
	{
	private:
		Test*& m_CurrentTest;
		std::vector<std::pair<std::string, std::function<std::unique_ptr<Test>()>>> m_Tests;
	public:
		TestMenu(Test*& currentTestPtr);
		~TestMenu();

		void OnImGuiRender() override;

		template <typename T, typename... Args>
		void AddTest(const std::string& name, Args&&... args) {
			std::cout << "Adding test: " << name << std::endl;
			m_Tests.push_back(std::make_pair(name,
				[args = std::make_tuple(std::forward<Args>(args)...)]() mutable -> std::unique_ptr<Test> {
					return std::apply([](auto&&... unpackedArgs) {
						return std::make_unique<T>(std::forward<decltype(unpackedArgs)>(unpackedArgs)...);
						}, std::move(args));
				}));
		}
	};
}