#pragma once

// cell size should be same as smothing radius for SPH, this is temp value
constexpr float gCellSize = 0.01f;

struct HashEntry{
    unsigned int hash;
    unsigned int particleIndex;
};
