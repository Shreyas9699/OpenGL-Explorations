#include "Test.h"
#include "imgui/imgui.h"

namespace test
{
	void TestMenu::OnImGuiRender()
	{
		for (auto& test : m_Tests)
		{
			if (ImGui::Button(test.first.c_str()))
				m_Selected = test.second();
		}
	}
}