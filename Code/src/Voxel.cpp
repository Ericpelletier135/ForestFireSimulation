#include "Voxel.h"
#include <iostream>
#include <random>
#include "FastNoiseLite.h"
#include <algorithm>

VoxelGrid::VoxelGrid(int width, int height, int depth, FireTerrain fireTerrain)
    : width(width), height(height), depth(depth), voxels(width * height * depth) {
    initGrid(fireTerrain);
}

Voxel& VoxelGrid::getVoxel(const glm::ivec3& pos) {
    return voxels[index(pos)];
}

int VoxelGrid::index(const glm::ivec3& pos) {
    int idx = pos.x + pos.y * width + pos.z * width * height;

    return idx;
}

void VoxelGrid::initGrid(FireTerrain fireTerrain) {
    std::random_device rd;
    std::mt19937 gen(rd());

    if (fireTerrain == CUBE) {

        // std::uniform_int_distribution<> initBurningDist(0, width * height * depth / 100);
        
        for (int z = 0; z < depth; ++z) {
            for (int y = 0; y < height; ++y) {
                for (int x = 0; x < width; ++x) {
                    glm::ivec3 pos(x, y, z);
                    Voxel& voxel = getVoxel(pos);
                    voxel.state = UNBURNED;
                    voxel.temperature = 20.0f;
                    voxel.fuel = 200.0f;
                    voxel.color = Voxel::getColorFromTemperature(voxel.temperature);

                    if ((z == 0 || z == depth-1) && (y == 0 || y == height-1) && (x == 0 || x == width-1)) {
                        voxel.state = BURNING;
                        voxel.temperature = 300.0f;
                        voxel.fuel -= 5.0f;
                        voxel.color = Voxel::getColorFromTemperature(voxel.temperature);
                    }

                    // if (initBurningDist(gen) == 0) {
                    //     voxel.state = BURNING;
                    //     voxel.temperature = 300.0f;
                    //     voxel.fuel -= 5.0f;
                    //     voxel.color = glm::vec4(Voxel::getColorFromTemperature(voxel.temperature), 0.8f);
                    // }

                }
            }
        }
    } else if (fireTerrain == PERLIN) {
        FastNoiseLite noiseGenerator;
        noiseGenerator.SetSeed(12345);
        noiseGenerator.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
        
        std::uniform_real_distribution<float> probabilityDist(0.0f, 1.0f);

        float burningThreshold = 0.5f; // proportion of burning voxels
        float emptyThreshold = -0.1f; // proportion of empty voxels
        float noiseScale = 6.5f; // size of the noise features
        bool first = true;
        for (int z = 0; z < depth; ++z) {
            for (int y = 0; y < height; ++y) {
                for (int x = 0; x < width; ++x) {
                    glm::ivec3 pos(x, y, z);
                    Voxel& voxel = getVoxel(pos);
                    
                    glm::vec3 noisePos = (glm::vec3(x, y, z)) * noiseScale;
                    float noiseValue = noiseGenerator.GetNoise(noisePos.x, noisePos.y, noisePos.z);

                    if (noiseValue < emptyThreshold) {
                        voxel.state = EMPTY;
                        voxel.temperature = 0.0f;
                        voxel.fuel = 0.0f;
                        voxel.color = Voxel::getColorFromTemperature(voxel.temperature);
                    } else {
                        voxel.state = UNBURNED;
                        voxel.temperature = 20.0f;
                        voxel.fuel = 200.0f;
                        voxel.color = Voxel::getColorFromTemperature(voxel.temperature);

                        // if (probabilityDist(gen) < burningThreshold && noiseValue > 0.0f) {
                        //     voxel.state = BURNING;
                        //     voxel.temperature = 300.0f;
                        //     voxel.fuel -= 5.0f;
                        //     voxel.color = Voxel::getColorFromTemperature(voxel.temperature);
                        // }
                        if (first && x == width - 1) {
                            voxel.state = BURNING;
                            voxel.temperature = 300.0f;
                            voxel.fuel -= 5.0f;
                            voxel.color = Voxel::getColorFromTemperature(voxel.temperature);
                            first = false;
                        }
                    }
                }
            }
        }
    } else if (fireTerrain == SPHERE) {
        int centerX = width / 2;
        int centerY = height / 2;
        int centerZ = depth / 2;
        int radius = std::min({width, height, depth}) / 2;

        for (int z = 0; z < depth; ++z) {
            for (int y = 0; y < height; ++y) {
                for (int x = 0; x < width; ++x) {
                    glm::ivec3 pos(x, y, z);
                    Voxel& voxel = getVoxel(pos);

                    // Calculate the distance from the center
                    int dx = x - centerX;
                    int dy = y - centerY;
                    int dz = z - centerZ;
                    int distanceSquared = dx * dx + dy * dy + dz * dz;

                    // Check if the voxel is inside the circular terrain
                    if (distanceSquared <= radius * radius) {
                        voxel.state = UNBURNED;
                        voxel.temperature = 20.0f;
                        voxel.fuel = 200.0f;
                        voxel.color = Voxel::getColorFromTemperature(voxel.temperature);

                        if (x == centerX && y == centerY && z == centerZ) {
                            voxel.state = BURNING;
                            voxel.temperature = 300.0f;
                            voxel.fuel -= 5.0f;
                            voxel.color = Voxel::getColorFromTemperature(voxel.temperature);
                        }
                    } else {
                        voxel.state = EMPTY;
                        voxel.temperature = 0.0f;
                        voxel.fuel = 0.0f;
                        voxel.color = Voxel::getColorFromTemperature(voxel.temperature);
                    }
                }
            }
        }
    } else if (fireTerrain == TEST) {
        bool first = true;
        for (int z = 0; z < depth; ++z) {
            for (int y = 0; y < height; ++y) {
                for (int x = 0; x < width; ++x) {
                    glm::ivec3 pos(x, y, z);
                    Voxel& voxel = getVoxel(pos);

                    voxel.state = UNBURNED;
                    voxel.temperature = 20.0f;
                    voxel.fuel = 200.0f;
                    voxel.color = Voxel::getColorFromTemperature(voxel.temperature);

                    if (first && x == width - 1) {
                        voxel.state = BURNING;
                        voxel.temperature = 300.0f;
                        voxel.fuel -= 5.0f;
                        voxel.color = Voxel::getColorFromTemperature(voxel.temperature);
                        first = false;
                    }
                }
            }
        }
    }
}


