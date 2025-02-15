#pragma once
#include <GLFW/glfw3.h>
#include "Camera.h"
#include <unordered_map>

class CameraController 
{
public:
    CameraController(GLFWwindow* window, Camera& camera);
    ~CameraController();

    void Update(float deltaTime);
    void SetEnabled(bool enabled) { m_enabled = enabled; }
    bool IsEnabled() const { return m_enabled; }
    float GetAspectRatio() const { return m_aspectRatio; }

    // Static callback wrappers
    static void MouseCallback(GLFWwindow* window, double xpos, double ypos);
    static void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);

private:
    static inline std::unordered_map<GLFWwindow*, CameraController*> s_controllers;

    // Instance methods for input handling
    void ProcessKeyboardInput(float deltaTime);
    void ProcessMouseMovement(double xpos, double ypos);
    void ProcessMouseScroll(double yoffset);

    // Window and camera references
    GLFWwindow* m_window;
    Camera& m_camera;

    // Mouse state
    float m_lastX;
    float m_lastY;
    bool m_firstMouse;

    // Control state
    bool m_enabled;
    float m_aspectRatio;

    // Previous callback functions (to maintain chain)
    GLFWcursorposfun m_previousMouseCallback;
    GLFWscrollfun m_previousScrollCallback;
};