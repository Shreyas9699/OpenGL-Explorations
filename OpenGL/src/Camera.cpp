#include "Camera.h"
#include <iostream>

glm::mat4 Camera::GetViewMatrix() const
{
    return glm::lookAt(Position, Position + Front, Up);
}

// processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
void Camera::ProcessKeyboard(Camera_Movement direction, float deltaTime)
{
    float velocity = MovementSpeed * deltaTime * outlier;
    if (direction == FORWARD)
        Position += Front * velocity;
    if (direction == BACKWARD)
        Position -= Front * velocity;
    if (direction == LEFT)
        Position -= Right * velocity;
    if (direction == RIGHT)
        Position += Right * velocity;
    if (direction == UP)
        Position += Up * velocity;
    if (direction == DOWN)
        Position -= Up * velocity;
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
