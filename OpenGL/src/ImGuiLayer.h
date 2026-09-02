#pragma once
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

class ImGuiLayer
{
public:
	ImGuiLayer(GLFWwindow* win);
	~ImGuiLayer();

	ImGuiLayer(const ImGuiLayer&)			 = delete;
	ImGuiLayer& operator=(const ImGuiLayer&) = delete;
	ImGuiLayer(ImGuiLayer&& o)				 = delete;
	ImGuiLayer& operator=(ImGuiLayer&& o)	 = delete;

	void Begin();
	void End();
};