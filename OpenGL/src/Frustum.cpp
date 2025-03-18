#include "Frustum.h"

void Frustum::Update(const glm::mat4& ProjView)
{
	glm::mat4 matrix = glm::transpose(ProjView);

	planes[0] = matrix[3] + matrix[0]; // Left
	planes[1] = matrix[3] - matrix[0]; // Right
	planes[2] = matrix[3] + matrix[1]; // Bottom
	planes[3] = matrix[3] - matrix[1]; // Top
	planes[4] = matrix[3] + matrix[2]; // Near
	planes[5] = matrix[3] - matrix[2]; // Far
	
	// Normalize
	for (int i = 0; i < 6; i++)
	{
        float length = glm::length(glm::vec3(planes[i].x, planes[i].y, planes[i].z));
		planes[i] /= length;
	}
}

bool Frustum::IsAABBVisible(const glm::vec3& min, const glm::vec3& max) const
{
	glm::vec3 center = (min + max) * 0.5f;
	glm::vec3 extents = max - center;

	for (const auto& plane : planes)
	{
		float dist = glm::dot(glm::vec3(plane), center) + plane.w;
		float radius = extents.x * std::abs(plane.x) + extents.y * std::abs(plane.y) + extents.z * std::abs(plane.z);
		if (dist + radius < 0.0f)
		{
			return false;
		}
	}
	return true;
}

bool Frustum::IsPointVisible(const glm::vec3& point) const
{
	for (const auto& plane : planes)
	{
		if (glm::dot(glm::vec3(plane), point) + plane.w < 0.0f)
		{
			return false;
		}
	}
	return true;
}

std::vector<glm::vec3> Frustum::GetCorners() const
{
    std::vector<glm::vec3> corners(8);

    // Compute the intersection of three planes to get each corner
    // The 8 corners are the intersections of the 6 planes

    // Function to compute the intersection of three planes
    auto intersectPlanes = [](const glm::vec4& p1, const glm::vec4& p2, const glm::vec4& p3) -> glm::vec3 {
        glm::vec3 n1(p1.x, p1.y, p1.z);
        glm::vec3 n2(p2.x, p2.y, p2.z);
        glm::vec3 n3(p3.x, p3.y, p3.z);

        glm::vec3 cross1 = glm::cross(n2, n3);
        glm::vec3 cross2 = glm::cross(n3, n1);
        glm::vec3 cross3 = glm::cross(n1, n2);

        float det = glm::dot(n1, cross1);

        if (std::abs(det) < 1e-6f) {
            // Planes are nearly parallel, return a default value
            return glm::vec3(0.0f);
        }

        glm::vec3 point = (-p1.w * cross1 - p2.w * cross2 - p3.w * cross3) / det;
        return point;
        };

    // Corner indices:
    // 0: near-left-bottom
    // 1: near-right-bottom
    // 2: near-right-top
    // 3: near-left-top
    // 4: far-left-bottom
    // 5: far-right-bottom
    // 6: far-right-top
    // 7: far-left-top

    corners[0] = intersectPlanes(planes[4], planes[0], planes[2]); // near-left-bottom
    corners[1] = intersectPlanes(planes[4], planes[1], planes[2]); // near-right-bottom
    corners[2] = intersectPlanes(planes[4], planes[1], planes[3]); // near-right-top
    corners[3] = intersectPlanes(planes[4], planes[0], planes[3]); // near-left-top
    corners[4] = intersectPlanes(planes[5], planes[0], planes[2]); // far-left-bottom
    corners[5] = intersectPlanes(planes[5], planes[1], planes[2]); // far-right-bottom
    corners[6] = intersectPlanes(planes[5], planes[1], planes[3]); // far-right-top
    corners[7] = intersectPlanes(planes[5], planes[0], planes[3]); // far-left-top

    return corners;
}