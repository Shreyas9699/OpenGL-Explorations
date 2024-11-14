#pragma once

#include "Timestep.h"
#include "Event.h"
#include <functional>
#include <vector>
#include <string>

namespace test 
{
	class Test
	{
	public:
		Test() {}
		virtual ~Test() {}
		
		virtual void OnUpdate(Timestep deltaTime) {}
		virtual void OnEvent(Event& e) {}
		virtual void OnRender() {}
		virtual void OnImGuiRender() {}
	};

	class TestMenu: public Test
	{
	private:
		Test*& m_CurrentTest;
		std::vector < std::pair<std::string, std::function<Test* ()>>> m_Tests;
	public:
		TestMenu(Test*& currentTestPtr);
		~TestMenu();

		void OnImGuiRender() override;

		template<typename T>
		void AddTest(const std::string& name)
		{
			//std::count << "Adding Test " << name << std::endl;
			m_Tests.push_back(std::make_pair(name, std::function<Test * ()>([]() { return new T(); })));
		}
	};
}