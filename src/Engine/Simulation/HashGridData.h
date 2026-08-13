#pragma once

// ToDo: Move this to an SPH / SHG spesific location
// cellSize >= smoothingRadius
constexpr float gCellSize = 5.0f;
constexpr float gSmoothingRadius = 2.5f;

struct HashEntry{
    unsigned int hash;
    unsigned int particleIndex;
};

struct HashLookupEntry{
    unsigned int hash;
    unsigned int cellIndex;
};