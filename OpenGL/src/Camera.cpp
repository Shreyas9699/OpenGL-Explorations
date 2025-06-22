#include "Camera.h"
#include <iostream>

glm::mat4 Camera::GetViewMatrix() const
{
    return glm::lookAt(Position, Position + Front, Up);
}

void Camera::LookAt(const glm::vec3& target)
{
    // Calculate new front vector
    glm::vec3 newFront = glm::normalize(target - Position);

    // Calculate new yaw and pitch
    glm::vec3 frontFlat = glm::normalize(glm::vec3(newFront.x, 0.0f, newFront.z));
    Yaw = glm::degrees(atan2(frontFlat.z, frontFlat.x)) - 90.0f;
    Pitch = glm::degrees(asin(newFront.y));

    updateCameraVectors(); // Update right/up vectors
}

// Camera constructor with vectors
Camera::Camera(
    glm::vec3 position,
    glm::vec3 up,
    glm::vec3 front,
    float yaw,
    float pitch,
    float movementSpeed,
    float mouseSens,
    float zoom)
    : Position(position),
      WorldUp(up),
      Front(front),
      Yaw(yaw),
      Pitch(pitch),
      MovementSpeed(movementSpeed),
      MouseSensitivity(mouseSens),
      Zoom(zoom)
{
    updateCameraVectors();
}

// Camera constructor with scalar values
Camera::Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch)
    : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
{
    Position = glm::vec3(posX, posY, posZ);
    WorldUp = glm::vec3(upX, upY, upZ);
    Yaw = yaw;
    Pitch = pitch;
    updateCameraVectors();
}

// processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
void Camera::ProcessKeyboard(Camera_Movement direction, float deltaTime)
{
    if (Mode == Camera_Mode::FreeFly) {
        if (SmoothMovement) {
            glm::vec3 desired = glm::vec3(0.0f);
            float velocity = MovementSpeed * outlier;
            switch (direction) {
                case Camera_Movement::FORWARD:  desired += Front * velocity; break;
                case Camera_Movement::BACKWARD: desired -= Front * velocity; break;
                case Camera_Movement::LEFT:     desired -= Right * velocity; break;
                case Camera_Movement::RIGHT:    desired += Right * velocity; break;
                case Camera_Movement::UP:       desired += Up * velocity; break;
                case Camera_Movement::DOWN:     desired -= Up * velocity; break;
            }
            // Accelerate towards desired direction
            Acceleration = (desired - Velocity) * Damping;
        } else {
            float velocity = MovementSpeed * deltaTime * outlier;
            switch (direction) {
                case Camera_Movement::FORWARD:  Position += Front * velocity; break;
                case Camera_Movement::BACKWARD: Position -= Front * velocity; break;
                case Camera_Movement::LEFT:     Position -= Right * velocity; break;
                case Camera_Movement::RIGHT:    Position += Right * velocity; break;
                case Camera_Movement::UP:       Position += Up * velocity; break;
                case Camera_Movement::DOWN:     Position -= Up * velocity; break;
            }
        }
    } else if (Mode == Camera_Mode::Orbit) {
        // Stub: Orbit mode movement (implement as needed)
        // For example, change OrbitDistance or OrbitTarget
    }
}

// processes input received from a mouse input system. Expects the offset value in both the x and y direction.
void Camera::ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch)
{
    xoffset *= MouseSensitivity;
    yoffset *= MouseSensitivity;

    Yaw += xoffset;
    Pitch += yoffset;

    // make sure that when pitch is out of bounds, screen doesn't get flipped
    if (constrainPitch)
    {
        if (Pitch > 89.0f)
            Pitch = 89.0f;
        if (Pitch < -89.0f)
            Pitch = -89.0f;
    }

    // update Front, Right and Up Vectors using the updated Euler angles
    updateCameraVectors();
}

// processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
void Camera::ProcessMouseScroll(float yoffset)
{
    Zoom -= (float)yoffset;
    if (Zoom < 1.0f)
        Zoom = 1.0f;
    if (Zoom > 45.0f)
        Zoom = 45.0f;
}

void Camera::UpdateCamera(float deltaTime)
{
    if (Mode == Camera_Mode::FreeFly && SmoothMovement) {
        // Integrate velocity and position
        Velocity += Acceleration * deltaTime;
        Position += Velocity * deltaTime;
        // Dampen velocity
        Velocity *= 0.95f;
        // Reset acceleration
        Acceleration = glm::vec3(0.0f);
    } else if (Mode == Camera_Mode::Orbit) {
        // Stub: Orbit mode update (implement as needed)
        // Example: update Position based on OrbitTarget, OrbitDistance, Yaw, Pitch
        float yawRad = glm::radians(Yaw);
        float pitchRad = glm::radians(Pitch);
        Position = OrbitTarget + glm::vec3(
            OrbitDistance * cos(pitchRad) * cos(yawRad),
            OrbitDistance * sin(pitchRad),
            OrbitDistance * cos(pitchRad) * sin(yawRad)
        );
        updateCameraVectors();
    }
}

void Camera::updateCameraVectors()
{
    // calculate the new Front vector
    glm::vec3 front;
    front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    front.y = sin(glm::radians(Pitch));
    front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    Front = glm::normalize(front);
    // also re-calculate the Right and Up vector
    Right = glm::normalize(glm::cross(Front, WorldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
    Up = glm::normalize(glm::cross(Right, Front));
}

void Camera::printCameraDetails() const
{
    std::cout << "----------------< Camera Details >----------------\n";
    std::cout << "Camera Position: ("
        << Position.x << ", "
        << Position.y << ", "
        << Position.z << ")\n";

    std::cout << "Camera Front: ("
        << Front.x << ", "
        << Front.y << ", "
        << Front.z << ")\n";

    std::cout << "Camera Up: ("
        << Up.x << ", "
        << Up.y << ", "
        << Up.z << ")\n";

    std::cout << "Camera Right: ("
        << Right.x << ", "
        << Right.y << ", "
        << Right.z << ")\n";

    std::cout << "Yaw: " << Yaw << "\n";
    std::cout << "Pitch: " << Pitch << "\n";
    std::cout << "Zoom: " << Zoom << "\n";
    std::cout << "--------------------------------\n";
}

void Camera::SetCameraZoom(float val) { Zoom = val; }

void Camera::IncreaseOutlier(float val) { outlier = val; }

void Camera::SetMouseSensitivity(float val) { MouseSensitivity = val; }
