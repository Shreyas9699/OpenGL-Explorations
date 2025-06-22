#pragma once
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// Camera movement types
enum class Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    UP,
    DOWN
};

// Camera control modes
enum class Camera_Mode {
    FreeFly,
    Orbit
};

// Default camera values
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 2.5f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;

class Camera {
public:
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;
    // euler Angles
    float Yaw;
    float Pitch;
    // camera options
    float MovementSpeed;
    float outlier = 1.0f;
    float MouseSensitivity;
    float Zoom;

    // Smoothing/acceleration
    bool SmoothMovement = false;
    glm::vec3 Velocity = glm::vec3(0.0f);
    glm::vec3 Acceleration = glm::vec3(0.0f);
    float Damping = 10.0f; // Higher = snappier

    // Camera mode
    Camera_Mode Mode = Camera_Mode::FreeFly;
    // Orbit/Arcball parameters (stubs)
    glm::vec3 OrbitTarget = glm::vec3(0.0f);
    float OrbitDistance = 10.0f;

    // constructor with vectors
    Camera(
        glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
        glm::vec3 front = glm::vec3(0.0f, 0.0f, -1.0f),
        float yaw = YAW,
        float pitch = PITCH,
        float movementSpeed = SPEED,
        float mouseSens = SENSITIVITY,
        float zoom = ZOOM
    );

    // constructor with scalar values
    Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch);

    // Camera movement methods
    glm::mat4 GetViewMatrix() const;
    void LookAt(const glm::vec3& target);
    void ProcessKeyboard(Camera_Movement direction, float deltaTime);
    void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true);
    void ProcessMouseScroll(float yoffset);
    void printCameraDetails() const;

    void SetCameraZoom(float val);
    void IncreaseOutlier(float val);
    void SetMouseSensitivity(float val);

    // Smoothing and mode controls
    void SetSmoothMovement(bool enable) { SmoothMovement = enable; }
    bool IsSmoothMovement() const { return SmoothMovement; }
    void SetCameraMode(Camera_Mode mode) { Mode = mode; }
    Camera_Mode GetCameraMode() const { return Mode; }
    void SetOrbitTarget(const glm::vec3& target) { OrbitTarget = target; }
    void SetOrbitDistance(float dist) { OrbitDistance = dist; }

    // Call this per-frame to update position if smoothing is enabled
    void UpdateCamera(float deltaTime);

private:
    void updateCameraVectors();
};