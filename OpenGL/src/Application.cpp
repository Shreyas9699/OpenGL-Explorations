
#include <GL/glew.h>
#include <GLFW/glfw3.h>
// refer to docs.gl to understand any function and its purpose

#include "Application.h"
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
#include "tests/TestLoadModels.h"

#include "tests/TestParticleCPU.h"
#include "tests/TestParticleGPU.h"
#include "tests/TestFireSimulation.h"

#include "tests/TestProceduralTerrain.h"

void Application::TestSetup()
{
    m_menu = std::make_unique<test::TestMenu>();

    m_menu->AddTest<test::TestClearColor>("Clear Color");
    m_menu->AddTest<test::TestTriangle>("Render 2D Traingle", m_win.get());
    m_menu->AddTest<test::TestSquare>("Render 2D Square", m_win.get());
    m_menu->AddTest<test::TestTexture2D>("2D Texture");
    m_menu->AddTest<test::TestPyramid>("3D Textured Pyramid", m_win->GetWindow());
    m_menu->AddTest<test::TestUVSphere>("Render Sphere", m_win->GetWindow());
    m_menu->AddTest<test::TestCubeRendering>("Material Cube Rendering", m_win->GetWindow());
    m_menu->AddTest<test::TestCubeWithTex>("Cube With Texture", m_win->GetWindow());
    m_menu->AddTest<test::TestFBMPlane>("Fractal Brownian Motion Plane", m_win.get());
    m_menu->AddTest<test::TestHeightMap>("Height Map", m_win.get());
    m_menu->AddTest<test::TestLoadModels>("Load Models", m_win.get());
    m_menu->AddTest<test::TestParticleCPU>("Particles System CPU", m_win.get());
    m_menu->AddTest<test::TestParticleGPU>("Particles System GPU", m_win.get());
    m_menu->AddTest<test::TestFireSimulation>("Fire Particles Sim", m_win.get()); //TestProceduralTerrain
    m_menu->AddTest<test::TestProceduralTerrain>("Procedural Terrain", m_win.get());
}

void Application::init()
{
    m_win->displayGPUDetails(true);
    TestSetup();
}

Application::Application(const std::string& title)
{
    m_win = std::make_unique<Window>(title);
    init();
}

Application::Application(const std::string& title, bool fullscreen)
{
    m_win = std::make_unique<Window>(title, fullscreen);
    init();
}

Application::Application(const std::string & title, int width, int height)
{
    m_win = std::make_unique<Window>(title, width, height);
    init();
}

Application::Application(const std::string & title, int width, int height, bool fullscreen)
{
    m_win = std::make_unique<Window>(title, width, height, fullscreen);
    init();
}

void Application::Run()
{
    m_imgui = std::make_unique<ImGuiLayer>(m_win->GetWindow());

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(m_win->GetWindow()))
    {
        bool goToMenu = false;
        Timestep dt = m_timer.Tick();

        // Check for backspace press and go back to menu
        if ( (glfwGetKey(m_win->GetWindow(), GLFW_KEY_BACKSPACE) == GLFW_PRESS) && m_currentTest)
        {
            goToMenu = true;
        }

        // Clear the screen
        GLCall(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
        GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

        m_imgui->Begin();

        test::Test* active = m_currentTest ? m_currentTest.get() : m_menu.get();
        if (active)
        {
            active->OnUpdate(dt, m_win->GetWindow());
            // Create your main rendering area window (no decorations)
            ImGui::Begin("Render View", nullptr,
                ImGuiWindowFlags_NoDecoration |
                ImGuiWindowFlags_NoInputs |
                ImGuiWindowFlags_NoBackground);
            active->OnRender();
            ImGui::End();

            ImGui::Begin("Test");
            if (m_currentTest && ImGui::Button("<- or Backspace"))
            {
                goToMenu = true;
            }
            active->OnImGuiRender();
            ImGui::End();
        }

        m_imgui->End();

        if (auto sel = m_menu->TakeSelected())
        {
            m_currentTest = std::move(sel);
        }

        if (goToMenu)
        {
            glfwSetInputMode(m_win->GetWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            m_currentTest.reset();
        }

        m_win->Update();
    }
}