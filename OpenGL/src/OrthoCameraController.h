#pragma once

// Class responsible for controlling an orthographic camera
#include "OrthoCamera.h"
#include "Event.h"
#include "Timestep.h"

class OrthoCameraController
{
public:
	OrthoCameraController(float aspectRatio, bool rotation = false);

	void OnUpdate(Timestep ts);
	void OnEvent(Event& e);

	OrthoCamera& GetCamera() { return m_Camera; }
	const OrthoCamera& GetCamera() const { return m_Camera; }

	float GetZoomLevel() const { return m_ZoomLevel; }
	void SetZoomLevel(float level) { m_ZoomLevel = level; }
private:
	bool OnMouseScrolled(MouseScrolledEvent& e);
	bool OnWindowResized(WindowResizeEvent& e);
private:
	float m_AspectRatio;
	float m_ZoomLevel = 1.0f;
	OrthoCamera m_Camera;

	bool m_Rotation;

	glm::vec3 m_CameraPosition = { 0.0f, 0.0f, 0.0f };
	float m_CameraRotation = 0.0f; //In degrees, in the anti-clockwise direction
	float m_CameraTranslationSpeed = 5.0f, m_CameraRotationSpeed = 180.0f;
};