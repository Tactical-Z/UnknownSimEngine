#pragma once

// cell size should be same as smothing radius for SPH, this is temp value
constexpr float gCellSize = 10.0f;

struct HashEntry{
    unsigned int hash;
    unsigned int particleIndex;
};

struct HashLookupEntry{
    unsigned int hash;
    unsigned int cellIndex;
};