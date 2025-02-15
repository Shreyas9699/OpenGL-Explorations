#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <functional> 

struct WindowProperties
{
    int xpos, ypos;
    int width, height;
};

class Window
{
private:
    std::string m_Title;
    bool m_Fullscreen;
    GLFWwindow* m_window;
    float m_AspectRatio;
    WindowProperties m_WindowedMode;
    std::function<void(int, int, int, int)> m_CustomKeyCallback;

    static void framebufferSizeCallback(GLFWwindow* window, int width, int height)
    {
        Window* windowInstance = static_cast<Window*>(glfwGetWindowUserPointer(window));
        if (windowInstance)
        {
            windowInstance->handleFramebufferResize(width, height);
        }
    }

    static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        Window* windowInstance = static_cast<Window*>(glfwGetWindowUserPointer(window));
        if (windowInstance)
        {
            windowInstance->handleKeyPress(key, scancode, action, mods);
        }
    }

public:
    Window(const std::string& title, WindowProperties winProp, bool fullscreen = false)
        : m_Title(title)
        , m_WindowedMode(winProp)
        , m_Fullscreen(fullscreen)
        , m_window(nullptr)
    {
        m_AspectRatio = float(winProp.width) / float(winProp.height);
    }

    void setCustomKeyCallback(std::function<void(int, int, int, int)> callback)
    {
        m_CustomKeyCallback = callback;
    }

    void toggleFullscreen()
    {
        m_Fullscreen = !m_Fullscreen;

        if (m_Fullscreen)
        {
            glfwGetWindowPos(m_window, &m_WindowedMode.xpos, &m_WindowedMode.ypos);
            glfwGetWindowSize(m_window, &m_WindowedMode.width, &m_WindowedMode.height);

            GLFWmonitor* monitor = glfwGetPrimaryMonitor();
            const GLFWvidmode* mode = glfwGetVideoMode(monitor);

            glfwSetWindowMonitor(m_window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
        }
        else
        {
            // Return to windowed mode with previous position and size
            glfwSetWindowMonitor(m_window, nullptr,
                m_WindowedMode.xpos, m_WindowedMode.ypos,
                m_WindowedMode.width, m_WindowedMode.height,
                0);
        }
        m_AspectRatio = static_cast<float>(m_WindowedMode.width) / static_cast<float>(m_WindowedMode.height);
    }

    void handleFramebufferResize(int width, int height)
    {
        m_WindowedMode.width = width;
        m_WindowedMode.height = height;
        m_AspectRatio = static_cast<float>(width) / static_cast<float>(height);
        glViewport(0, 0, width, height);
    }

    void handleKeyPress(int key, int scancode, int action, int mods)
    {
        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(m_window, true);
        }
        else if (key == GLFW_KEY_F11 && action == GLFW_PRESS)
        {
            toggleFullscreen();
        }
        if (m_CustomKeyCallback)
        {
            m_CustomKeyCallback(key, scancode, action, mods);
        }
    }

    bool init()
    {
        if (!glfwInit())
        {
            std::cerr << "Failed to initialize GLFW!" << std::endl;
            return false;
        }
        m_window = glfwCreateWindow(m_WindowedMode.width, m_WindowedMode.height, m_Title.c_str(),
            m_Fullscreen ? glfwGetPrimaryMonitor() : nullptr, nullptr);

        if (!m_window)
        {
            std::cerr << "Failed to create GLFW window!" << std::endl;
            glfwTerminate();
            return false;
        }

        glfwMakeContextCurrent(m_window);
        glfwSwapInterval(0);

        // Enable OpenGL features
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_MULTISAMPLE);

        glfwSetWindowUserPointer(m_window, this);

        // Use lambda functions to bind the instance
        glfwSetFramebufferSizeCallback(m_window, [](GLFWwindow* window, int width, int height)
            {
                Window* windowInstance = static_cast<Window*>(glfwGetWindowUserPointer(window));
                if (windowInstance)
                {
                    windowInstance->handleFramebufferResize(width, height);
                }
            });

        glfwSetKeyCallback(m_window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
            Window* windowInstance = static_cast<Window*>(glfwGetWindowUserPointer(window));
            if (windowInstance)
            {
                windowInstance->handleKeyPress(key, scancode, action, mods);
            }
            });

        //glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        if (glewInit() != GLEW_OK)
        {
            std::cerr << "Failed to initialize GLEW!" << std::endl;
            glfwTerminate();
            return false;
        }
        glEnable(GL_DEPTH_TEST);
        return true;
    }

    void OnUpdate()
    {
        glfwPollEvents();
        glfwSwapBuffers(m_window);
    }

    float getAspectRatio() const { return m_AspectRatio; }
    GLFWwindow* getWindow() const { return m_window; }
    void setKeyCallback(GLFWkeyfun callback) { glfwSetKeyCallback(m_window, callback); }
    void setScrollCallback(GLFWscrollfun callback) { glfwSetScrollCallback(m_window, callback); }
    void setCursorPositionCallback(GLFWcursorposfun callback) { glfwSetCursorPosCallback(m_window, callback); }

    ~Window()
    {
        if (m_window)
        {
            glfwDestroyWindow(m_window);
            m_window = nullptr;
        }
        glfwTerminate();
    }
};