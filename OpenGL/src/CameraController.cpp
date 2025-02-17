#include "CameraController.h"

CameraController::CameraController(GLFWwindow* window, Camera& camera)
    : m_window(window)
    , m_camera(camera)
    , m_lastX(0.0f)
    , m_lastY(0.0f)
    , m_firstMouse(true)
    , m_enabled(true)
{
    // Store window dimensions for initial aspect ratio
    int width, height;
    glfwGetWindowSize(window, &width, &height);
    m_aspectRatio = static_cast<float>(width) / static_cast<float>(height);

    // Store previous callbacks
    m_previousMouseCallback = glfwSetCursorPosCallback(window, MouseCallback);
    m_previousScrollCallback = glfwSetScrollCallback(window, ScrollCallback);

    s_controllers[window] = this;
}

CameraController::~CameraController() 
{
    s_controllers.erase(m_window);
    glfwSetCursorPosCallback(m_window, m_previousMouseCallback);
    glfwSetScrollCallback(m_window, m_previousScrollCallback);
}

void CameraController::Update(float deltaTime) 
{
    if (!m_enabled) return;
    ProcessKeyboardInput(deltaTime);
}

void CameraController::ProcessKeyboardInput(float deltaTime) 
{
    if (glfwGetKey(m_window, GLFW_KEY_W) == GLFW_PRESS)
        m_camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(m_window, GLFW_KEY_S) == GLFW_PRESS)
        m_camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(m_window, GLFW_KEY_A) == GLFW_PRESS)
        m_camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(m_window, GLFW_KEY_D) == GLFW_PRESS)
        m_camera.ProcessKeyboard(RIGHT, deltaTime);
    if (glfwGetKey(m_window, GLFW_KEY_SPACE) == GLFW_PRESS)
        m_camera.ProcessKeyboard(UP, deltaTime);
    if (glfwGetKey(m_window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS)
        m_camera.ProcessKeyboard(DOWN, deltaTime);

    static bool isCPressed = false;
    if (glfwGetKey(m_window, GLFW_KEY_C) == GLFW_PRESS) 
    {
        if (!isCPressed) 
        {
            m_camera.printCameraDetails();
            isCPressed = true;
        }
    }
    else 
    {
        isCPressed = false;
    }    
}

void CameraController::ProcessMouseMovement(double xpos, double ypos) 
{
    if (!m_enabled) return;

    if (m_firstMouse) 
    {
        m_lastX = (float)xpos;
        m_lastY = (float)ypos;
        m_firstMouse = false;
    }

    float xoffset = (float)xpos - m_lastX;
    float yoffset = m_lastY - (float)ypos;

    m_lastX = (float)xpos;
    m_lastY = (float)ypos;

    m_camera.ProcessMouseMovement(xoffset, yoffset);
}

void CameraController::ProcessMouseScroll(double yoffset) 
{
    if (!m_enabled) return;
    m_camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

// Static callback implementations
void CameraController::MouseCallback(GLFWwindow* window, double xpos, double ypos) 
{
    if (auto it = s_controllers.find(window); it != s_controllers.end()) 
    {
        it->second->ProcessMouseMovement(xpos, ypos);
    }
}

void CameraController::ScrollCallback(GLFWwindow* window, double xoffset, double yoffset) 
{
    if (auto it = s_controllers.find(window); it != s_controllers.end()) 
    {
        it->second->ProcessMouseScroll(yoffset);
    }
}