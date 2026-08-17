#pragma once
#include <glm/glm.hpp>
#include <array>
#include <vector>

const unsigned int NUM_FRUSTUM_FILL_VERTS = 36;

class Frustum
{
public:
    void Update(const glm::mat4& viewProj);
    bool IsAABBVisible(const glm::vec3& min, const glm::vec3& max) const;
    bool IsPointVisible(const glm::vec3& point) const;
    std::vector<glm::vec3> GetCorners() const;

    const std::array<glm::vec4, 6>& GetPlanes() const { return planes; }
private:
    //std::vector<glm::vec3> m_FrustumCorners;
    std::array<glm::vec4, 6> planes;
};