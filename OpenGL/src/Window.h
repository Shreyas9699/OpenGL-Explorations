#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <functional> 

class Window
{
public:
	// Constructor 1: Just title (default sizing, windowed mode)
	Window(const std::string& title = "OpenGL Application")
		: m_Title(title), m_Fullscreen(false) 
	{
		initialize();
	}

	// Constructor 2: Title and fullscreen flag
	Window(const std::string& title, bool fullscreen)
		: m_Title(title), m_Fullscreen(fullscreen) 
	{
		initialize();
	}

	// Constructor 3: Title and custom dimensions
	Window(const std::string& title, int width, int height)
		: m_Title(title), m_Fullscreen(false), m_WindowWidth(width), m_WindowHeight(height) 
	{
		initialize();
	}

	// Constructor 4: Title, custom dimensions, and fullscreen flag
	Window(const std::string& title, int width, int height, bool fullscreen)
		: m_Title(title), m_Fullscreen(fullscreen), m_WindowWidth(width), m_WindowHeight(height) 
	{
		initialize();
	}

	~Window()
	{
		if (m_Window)
		{
			glfwDestroyWindow(m_Window);
			m_Window = nullptr;
		}
		glfwTerminate();
	}

	Window& setSize(int width, int height)
	{
		// This would need to resize an existing window
		if (m_Window) {
			glfwSetWindowSize(m_Window, width, height);
			m_WindowWidth = width;
			m_WindowHeight = height;
			m_AspectRatio = static_cast<float>(width) / static_cast<float>(height);
		}
		return *this;
	}

	Window& setHint(int hint, int value)
	{
		m_WindowHints[hint] = value;
		return *this;
	}

	void displayGPUDetails()
	{
		std::cout << "Vendor: " << glGetString(GL_VENDOR) << std::endl;
		std::cout << "GPU: " << glGetString(GL_RENDERER) << std::endl;
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
			glfwGetWindowPos(m_Window, &m_WindowX, &m_WindowY);
			glfwGetWindowSize(m_Window, &m_WindowWidth,&m_WindowHeight);

			GLFWmonitor* monitor = glfwGetPrimaryMonitor();
			const GLFWvidmode* mode = glfwGetVideoMode(monitor);

			glfwSetWindowMonitor(m_Window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
		}
		else
		{
			// Return to windowed mode with previous position and size
			glfwSetWindowMonitor(
				m_Window, nullptr,
				m_WindowX, m_WindowY,
				m_WindowWidth, m_WindowHeight,
				0);
		}
		m_AspectRatio = static_cast<float>(m_WindowWidth) / static_cast<float>(m_WindowHeight);
	}

	void handleFramebufferResize(int width, int height)
	{
		m_WindowWidth = width;
		m_WindowHeight = height;
		m_AspectRatio = static_cast<float>(width) / static_cast<float>(height);
		glViewport(0, 0, width, height);

		std::cout << "Resized window size is: " << width << "x" << height << std::endl;
		std::cout << "Aspect ratio: " << m_AspectRatio << std::endl;
	}

	void handleKeyPress(int key, int scancode, int action, int mods)
	{
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		{
			glfwSetWindowShouldClose(m_Window, true);
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

	float GetAspectRatio() const 
	{ 
		return m_AspectRatio;
	}

	GLFWwindow* GetWindow() const 
	{ 
		return m_Window; 
	}

	void setKeyCallback(GLFWkeyfun callback) 
	{ 
		glfwSetKeyCallback(m_Window, callback); 
	}

	void setScrollCallback(GLFWscrollfun callback) 
	{ 
		glfwSetScrollCallback(m_Window, callback); 
	}

	void setCursorPositionCallback(GLFWcursorposfun callback) 
	{ 
		glfwSetCursorPosCallback(m_Window, callback); 
	}

	void Update()
	{
		glfwPollEvents();
		glfwSwapBuffers(m_Window);
	}

private:
	std::string m_Title;
	bool m_Fullscreen;
	GLFWwindow* m_Window;
	float m_AspectRatio;
	int m_WindowX = 100, m_WindowY = 100;
	int m_WindowWidth = -1, m_WindowHeight = -1;

	std::function<void(int, int, int, int)> m_CustomKeyCallback;
	std::unordered_map<int, int> m_WindowHints;

	void initialize()
	{
		// Set default hints
		m_WindowHints[GLFW_SAMPLES] = 4;
		m_WindowHints[GLFW_CONTEXT_VERSION_MAJOR] = 4;
		m_WindowHints[GLFW_CONTEXT_VERSION_MINOR] = 2;
		m_WindowHints[GLFW_OPENGL_PROFILE] = GLFW_OPENGL_CORE_PROFILE;

		// Initialize GLFW
		if (!glfwInit())
		{
			throw std::runtime_error("Failed to initialize GLFW!");
		}

		// Apply hints
		for (const auto& hint : m_WindowHints)
		{
			glfwWindowHint(hint.first, hint.second);
		}

		if (m_WindowWidth <= 0 || m_WindowHeight <= 0)
		{
			const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
			if (mode)
			{
				m_WindowWidth = static_cast<int>(mode->width * 0.75f);
				m_WindowHeight = static_cast<int>(mode->height * 0.75f);
			}
			else
			{
				m_WindowWidth = 1280;
				m_WindowHeight = 720;
			}
		}

		m_AspectRatio = static_cast<float>(m_WindowWidth) / static_cast<float>(m_WindowHeight);

		// Creating window
		m_Window = glfwCreateWindow(
			m_WindowWidth,
			m_WindowHeight,
			m_Title.c_str(),
			m_Fullscreen ? glfwGetPrimaryMonitor() : nullptr,
			nullptr);

		if (!m_Window)
		{
			glfwTerminate();
			throw std::runtime_error("Failed to create GLFW window!");
		}

		glfwMakeContextCurrent(m_Window);
		glfwSwapInterval(0);

		// Set up callbacks
		glfwSetWindowUserPointer(m_Window, this);
		glfwSetFramebufferSizeCallback(m_Window, framebufferSizeCallback);
		glfwSetKeyCallback(m_Window, keyCallback);

		// Initialize GLEW
		if (glewInit() != GLEW_OK)
		{
			glfwTerminate();
			throw std::runtime_error("Failed to initialize GLEW!");
		}
	}

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
};