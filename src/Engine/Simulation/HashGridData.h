#pragma once


constexpr glm::vec3 gGridBoundsMin = glm::vec3(-25);
constexpr glm::vec3 gGridBoundsMax = glm::vec3(25);
constexpr float gCellSize = 5.0f;
const glm::ivec3 gGridSize = glm::ivec3(glm::ceil((gGridBoundsMax - gGridBoundsMin) / gCellSize));
const int gCellCount = gGridSize.x * gGridSize.y * gGridSize.z;
// cellSize >= smoothingRadius
constexpr float gSmoothingRadius = 2.5f;

struct HashEntry{
    unsigned int mHash;
    unsigned int mIndex;
};