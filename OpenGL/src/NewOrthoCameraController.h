// NewOrthoCameraController.h
#pragma once

#include "Event.h"
#include "Timestep.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class OrthographicCamera
{
public:
    OrthographicCamera(float left, float right, float bottom, float top)
        : m_ProjectionMatrix(glm::ortho(left, right, bottom, top, -1.0f, 1.0f))
    {
        m_ViewMatrix = glm::mat4(1.0f);
        m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
    }

    void SetProjection(float left, float right, float bottom, float top)
    {
        m_ProjectionMatrix = glm::ortho(left, right, bottom, top, -1.0f, 1.0f);
        m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
    }

    const glm::vec3& GetPosition() const { return m_Position; }
    void SetPosition(const glm::vec3& position)
    {
        m_Position = position;
        RecalculateViewMatrix();
    }

    float GetRotation() const { return m_Rotation; }
    void SetRotation(float rotation)
    {
        m_Rotation = rotation;
        RecalculateViewMatrix();
    }

    const glm::mat4& GetProjectionMatrix() const { return m_ProjectionMatrix; }
    const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }
    const glm::mat4& GetViewProjectionMatrix() const { return m_ViewProjectionMatrix; }

private:
    void RecalculateViewMatrix()
    {
        glm::mat4 transform = glm::translate(glm::mat4(1.0f), m_Position) *
            glm::rotate(glm::mat4(1.0f), glm::radians(m_Rotation), glm::vec3(0, 0, 1));

        m_ViewMatrix = glm::inverse(transform);
        m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
    }

private:
    glm::mat4 m_ProjectionMatrix;
    glm::mat4 m_ViewMatrix;
    glm::mat4 m_ViewProjectionMatrix;

    glm::vec3 m_Position = { 0.0f, 0.0f, 0.0f };
    float m_Rotation = 0.0f;
};

class OrthographicCameraController
{
public:
    OrthographicCameraController(float aspectRatio)
        : m_AspectRatio(aspectRatio),
        m_Camera(-m_AspectRatio * m_ZoomLevel, m_AspectRatio* m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel)
    {
    }

    void OnUpdate(Timestep ts)
    {
        if (m_KeyPressed[GLFW_KEY_A])
            m_CameraPosition.x -= m_CameraTranslationSpeed * ts;
        else if (m_KeyPressed[GLFW_KEY_D])
            m_CameraPosition.x += m_CameraTranslationSpeed * ts;

        if (m_KeyPressed[GLFW_KEY_W])
            m_CameraPosition.y += m_CameraTranslationSpeed * ts;
        else if (m_KeyPressed[GLFW_KEY_S])
            m_CameraPosition.y -= m_CameraTranslationSpeed * ts;

        if (m_KeyPressed[GLFW_KEY_Q])
            m_CameraRotation += m_CameraRotationSpeed * ts;
        else if (m_KeyPressed[GLFW_KEY_E])
            m_CameraRotation -= m_CameraRotationSpeed * ts;

        m_Camera.SetPosition(m_CameraPosition);
        m_Camera.SetRotation(m_CameraRotation);

        m_CameraTranslationSpeed = m_ZoomLevel;
    }

    void OnEvent(Event& e)
    {
        EventDispatcher dispatcher(e);
        dispatcher.Dispatch<KeyPressedEvent>(std::bind(&OrthographicCameraController::OnKeyPressed, this, std::placeholders::_1));
        dispatcher.Dispatch<KeyReleasedEvent>(std::bind(&OrthographicCameraController::OnKeyReleased, this, std::placeholders::_1));
        dispatcher.Dispatch<MouseScrolledEvent>(std::bind(&OrthographicCameraController::OnMouseScrolled, this, std::placeholders::_1));
        dispatcher.Dispatch<WindowResizeEvent>(std::bind(&OrthographicCameraController::OnWindowResized, this, std::placeholders::_1));
    }

    OrthographicCamera& GetCamera() { return m_Camera; }
    const OrthographicCamera& GetCamera() const { return m_Camera; }

    float GetZoomLevel() const { return m_ZoomLevel; }
    void SetZoomLevel(float level)
    {
        m_ZoomLevel = level;
        m_Camera.SetProjection(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel);
    }

private:
    bool OnKeyPressed(KeyPressedEvent& e)
    {
        m_KeyPressed[e.GetKeyCode()] = true;
        return false;
    }

    bool OnKeyReleased(KeyReleasedEvent& e)
    {
        m_KeyPressed[e.GetKeyCode()] = false;
        return false;
    }

    bool OnMouseScrolled(MouseScrolledEvent& e)
    {
        m_ZoomLevel -= e.GetYOffset() * 0.25f;
        m_ZoomLevel = std::max(m_ZoomLevel, 0.25f);
        m_Camera.SetProjection(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel);
        return false;
    }

    bool OnWindowResized(WindowResizeEvent& e)
    {
        m_AspectRatio = (float)e.GetWidth() / (float)e.GetHeight();
        m_Camera.SetProjection(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel);
        return false;
    }

private:
    float m_AspectRatio;
    float m_ZoomLevel = 1.0f;
    OrthographicCamera m_Camera;

    glm::vec3 m_CameraPosition = { 0.0f, 0.0f, 0.0f };
    float m_CameraRotation = 0.0f;

    float m_CameraTranslationSpeed = 5.0f;
    float m_CameraRotationSpeed = 180.0f;

    std::unordered_map<int, bool> m_KeyPressed;
};