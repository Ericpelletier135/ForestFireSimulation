// Voxel.h
#ifndef VOXEL_H
#define VOXEL_H

#include <vector>
#include <glm/glm.hpp>

enum VoxelState {
    UNBURNED,
    BURNING,
    BURNED_OUT,
    EMPTY
};

enum FireTerrain {
    CUBE,
    PERLIN,
    SPHERE,
    TEST
};

struct Voxel {
    VoxelState state;
    float temperature;
    float fuel;
    glm::vec4 color;

    static glm::vec4 getColorFromTemperature(float temperature) {
        if (temperature <= 0.0f)
            return glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
        float t = std::max(0.0f, std::min(temperature / 1000.0f, 1.0f));

        // Define the start (cold) and end (hot) colors
        glm::vec3 blueColor(0.0f, 0.0f, 1.0f); // Blue
        glm::vec3 redColor(1.0f, 0.0f, 0.0f);  // Red
        glm::vec3 yellowColor(1.0f, 1.0f, 0.0f); // Yellow

        glm::vec4 color;

        // First half of the temperature range: transition from blue to red
        if (t <= 0.5f) {
            float tNormalized = t / 0.5f;
            color = glm::vec4(glm::mix(blueColor, redColor, tNormalized), t);
        }
        // Second half of the temperature range: transition from red to yellow
        else {
            float tNormalized = (t - 0.5f) / 0.5f;
            color = glm::vec4(glm::mix(redColor, yellowColor, tNormalized), t);
        }

        return color;
    }
};

class VoxelGrid {
public:
    VoxelGrid(int width, int height, int depth, FireTerrain fireTerrain);

    Voxel& getVoxel(const glm::ivec3& pos);

    int index(const glm::ivec3& pos);
    void initGrid(FireTerrain fireTerrain);

    int width;
    int height;
    int depth;
    std::vector<Voxel> voxels;
};

#endif // VOXEL_H


