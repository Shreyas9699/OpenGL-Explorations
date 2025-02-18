#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include "tests/TestClearColor.h"
#include "tests/TestTriangle.h"
#include "tests/TestTexture2D.h"
#include "tests/TestPyramid.h"
#include "tests/TestUVSphere.h"
#include "tests/TestCubeRendering.h"
#include "tests/TestSquare.h"
#include "tests/TestCubeRenderingWTex.h"
#include "tests/TestFBMPlane.h"
#include "tests/TestHeightMap.h"

#include "Window.h"
// refer to docs.gl to understand any function and its purpose

int main(void)
{
    WindowProperties windowedMode = { 100, 100, 920, 900 };
    Window window("Hello World", windowedMode, false);
    if (!window.init())
    {
        return -1;
    }

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window.getWindow(), true);
    ImGui_ImplOpenGL3_Init();

    test::Test* currentTest = nullptr;
    test::TestMenu* menu = new test::TestMenu(currentTest);
    currentTest = menu;


    menu->AddTest<test::TestClearColor>("Clear Color");
    menu->AddTest<test::TestTriangle>("Render 2D Traingle");
    menu->AddTest<test::TestSquare>("Render 2D Square");
    menu->AddTest<test::TestTexture2D>("2D Texture");
    menu->AddTest<test::TestPyramid>("3D Textured Pyramid", window.getWindow());
    menu->AddTest<test::TestUVSphere>("Render Sphere", window.getWindow());
    menu->AddTest<test::TestCubeRendering>("Material Cube Rendering", window.getWindow());
    menu->AddTest<test::TestCubeWithTex>("Cube With Texture", window.getWindow());
    menu->AddTest<test::TestFBMPlane>("Fractal Brownian Motion Plane", &window);
    menu->AddTest<test::TestHeightMap>("Height Map", &window);

    float lastFrameTime = 0.0f;
    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window.getWindow()))
    {
        float currentFrameTime = float(glfwGetTime());
        float deltaTime = currentFrameTime - lastFrameTime;
        lastFrameTime = currentFrameTime;

        // Check for backspace press and go back to menu
        if (glfwGetKey(window.getWindow(), GLFW_KEY_BACKSPACE) == GLFW_PRESS)
        {
            if (currentTest && currentTest != menu)
            {
                glfwSetInputMode(window.getWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                delete currentTest;
                currentTest = menu;
            }
        }

        // Clear the screen
        GLCall(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
        GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        
        if (currentTest)
        {
            currentTest->OnUpdate(deltaTime, window.getWindow());
            currentTest->OnRender();
            ImGui::Begin("Test");
            if (currentTest != menu && ImGui::Button("<- or Backspace"))
            {
                glfwSetInputMode(window.getWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                delete currentTest;
                currentTest = menu;
            }
            currentTest->OnImGuiRender();
            ImGui::End();
        }

        // IMGUI Rendering
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            GLFWwindow* backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }

        window.OnUpdate();
    }

    if (currentTest != menu)
        delete menu;
    delete currentTest;

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();


    return 0;
}