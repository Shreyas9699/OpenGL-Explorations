#pragma once
#include <string>

#include "Window.h"
#include "Timer.h"
#include "ImGuiLayer.h"
#include "tests/Test.h"


class Application
{
public:
    std::unique_ptr<Window> m_win;
    std::unique_ptr<ImGuiLayer> m_imgui;

    Application(const std::string& title = "");
    Application(const std::string& title, bool fullscreen);
    Application(const std::string& title, int width, int height);
    Application(const std::string& title, int width, int height, bool fullscreen);

    Application(const Application&)            = delete;
    Application& operator=(const Application&) = delete;
    Application(Application&& o)               = delete;
    Application& operator=(Application&& o)    = delete;

    void init();
    void Run();

private:
    std::unique_ptr<test::Test> m_currentTest = nullptr;
    std::unique_ptr<test::TestMenu> m_menu = nullptr;
    Timer m_timer;

    void TestSetup();
};