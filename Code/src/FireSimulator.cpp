// FireSimulator.cpp
#include "FireSimulator.h"
#include <iostream>

float probabilityScale = 100000.0f;

FireSimulator::FireSimulator(int width, int height, int depth, FireTerrain fireTerrain)
    : voxelGrid(width, height, depth, fireTerrain) {}

std::pair<bool, bool> FireSimulator::update(float deltaTime, const glm::vec3& windDirection, float windIntensity, float humidity, float diffusionRate, float burningRate, float ignitionRate, float coolingRate, float temperatureIncreaseRate) {
    VoxelGrid newVoxelGrid = voxelGrid; // Create a copy of the current voxel grid

    bool burning = false;
    bool allIgnited = true;

    for (int z = 0; z < voxelGrid.depth; ++z) {
        for (int y = 0; y < voxelGrid.height; ++y) {
            for (int x = 0; x < voxelGrid.width; ++x) {
                glm::ivec3 pos(x, y, z);
                Voxel& voxel = voxelGrid.getVoxel(pos);
                Voxel& newVoxel = newVoxelGrid.getVoxel(pos);

                if (voxel.state == EMPTY) {
                    continue;
                }

                if (voxel.state == BURNING) {
                    // at least one voxel burning
                    burning = true;

                    // Update voxel temperature
                    newVoxel.temperature = voxel.temperature + deltaTime * temperatureIncreaseRate;

                    // Update fuel if the voxel is burning
                    if (voxel.fuel > 0) {
                        newVoxel.fuel = std::max(0.0f, voxel.fuel - deltaTime * burningRate * 100.0f);
                        newVoxel.temperature = voxel.temperature + deltaTime * temperatureIncreaseRate * (1.0f - (newVoxel.fuel / 200.0f));
                        newVoxel.color = Voxel::getColorFromTemperature(voxel.temperature);
                    } else {
                        newVoxel.state = BURNED_OUT;
                        newVoxel.temperature = std::max(0.0f, voxel.temperature - deltaTime * coolingRate);
                        newVoxel.color = Voxel::getColorFromTemperature(voxel.temperature);
                    }

                    // Spread fire to neighbors
                    for (int dz = -1; dz <= 1; ++dz) {
                        for (int dy = -1; dy <= 1; ++dy) {
                            for (int dx = -1; dx <= 1; ++dx) {
                                if (dx == 0 && dy == 0 && dz == 0) continue; // Skip the current voxel

                                glm::ivec3 neighborPos(x + dx, y + dy, z + dz);

                                // Check if the neighbor position is within the grid bounds
                                if (neighborPos.x >= 0 && neighborPos.x < voxelGrid.width &&
                                    neighborPos.y >= 0 && neighborPos.y < voxelGrid.height &&
                                    neighborPos.z >= 0 && neighborPos.z < voxelGrid.depth) {

                                    Voxel& neighbor = voxelGrid.getVoxel(neighborPos);
 
                                    if (neighbor.state == UNBURNED && neighbor.fuel > 0) {
                                        // Compute diffusion probability based on the temperature difference between the voxel and the neighbor
                                        float windInfluence = 0.0f;
                                        if (windDirection != glm::vec3(0.0f, 0.0f, 0.0f)) {
                                            windInfluence = glm::dot(glm::normalize(windDirection), glm::normalize(glm::vec3(dx, dy, dz))) * windIntensity;
                                        }
                                        float diffusionProbability = diffusionRate * (newVoxel.temperature - neighbor.temperature) * (1.0f + windInfluence);
                                        
                                        if (diffusionProbability > 0.0f && rand() / (float)RAND_MAX * probabilityScale < diffusionProbability) {
                                            // Diffuse fire to the neighbor
                                            Voxel& newNeighbor = newVoxelGrid.getVoxel(neighborPos);
                                            newNeighbor.state = BURNING;
                                            newNeighbor.temperature = voxel.temperature;
                                            newNeighbor.fuel -= 5.0f;
                                            newNeighbor.color = Voxel::getColorFromTemperature(voxel.temperature);

                                            // Check if the newNeighbor has run out of fuel
                                            if (newNeighbor.fuel <= 0.0f) {
                                                newNeighbor.state = BURNED_OUT;
                                                newNeighbor.fuel = 0.0f;
                                                newNeighbor.color = Voxel::getColorFromTemperature(voxel.temperature);
                                            }
                                        }

                                    }

                                    // Compute ignition probability based on the temperature and fuel of the neighbor
                                    float ignitionProbability = ignitionRate * neighbor.fuel * (1.0f - neighbor.temperature / 1000.0f);

                                    if (neighbor.state == UNBURNED && rand() / (float)RAND_MAX * probabilityScale < ignitionProbability) {
                                        // Ignite the neighbor if it's unburned and the ignition probability is high enough
                                        Voxel& newNeighbor = newVoxelGrid.getVoxel(neighborPos);
                                        newNeighbor.state = BURNING;
                                        newNeighbor.temperature = 300.0f;
                                        newNeighbor.fuel -= 5.0f;
                                        newNeighbor.color = Voxel::getColorFromTemperature(voxel.temperature);

                                        // Check if the newNeighbor has run out of fuel
                                        if (newNeighbor.fuel <= 0.0f) {
                                            newNeighbor.state = BURNED_OUT;
                                            newNeighbor.fuel = 0.0f;
                                            newNeighbor.color = Voxel::getColorFromTemperature(voxel.temperature);
                                        }
                                    }

                                }
                            }
                        }
                    }
                } else if (voxel.state == BURNED_OUT) {
                    // Decrease temperature and update color for burned-out voxels
                    newVoxel.temperature = std::max(0.0f, voxel.temperature - deltaTime * coolingRate);
                    newVoxel.color = Voxel::getColorFromTemperature(voxel.temperature);
                }

                if (voxel.state == UNBURNED) {
                    allIgnited = false;
                }
               
            }
        }
    }

    // Swap the voxel grids
    voxelGrid = newVoxelGrid;

    return std::make_pair(burning, allIgnited);
}


Voxel& FireSimulator::getVoxel(const glm::ivec3& pos) {
    return voxelGrid.getVoxel(pos);
}