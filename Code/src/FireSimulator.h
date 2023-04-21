// FireSimulator.h
#ifndef FIRESIMULATOR_H
#define FIRESIMULATOR_H

#include "Voxel.h"
#include <glm/glm.hpp>
#include <utility>

class FireSimulator {
public:
    FireSimulator(int width, int height, int depth, FireTerrain fireTerrain);

    std::pair<bool, bool> update(float deltaTime, const glm::vec3& windDirection, float windIntensity, float humidity, float diffusionRate, float burningRate, float ignitionRate, float coolingRate, float temperatureIncreaseRate);

    Voxel& getVoxel(const glm::ivec3& pos);

    VoxelGrid voxelGrid;
    
    // Define wind direction and humidity
    glm::vec3 windDirection; // Example: wind blowing in the positive X direction
    float windIntensity; // Example: 50% wind intensity
    float humidity; // Example: 30% humidity
    float diffusionRate; // Diffusion rate
    float burningRate; // Burning rate
    float ignitionRate; // Ignition rate
    float coolingRate; // Cooling rate


};

#endif // FIRESIMULATOR_H
