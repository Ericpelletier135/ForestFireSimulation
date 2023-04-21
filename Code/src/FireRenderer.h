// FireRenderer.h
#ifndef FIRERENDERER_H
#define FIRERENDERER_H

#include "Shader.h"
#include "Voxel.h"
#include "FireSimulator.h"

class FireRenderer {
public:
    FireRenderer(FireSimulator& fireSimulator);
    ~FireRenderer();

    void render(const glm::mat4& view, const glm::mat4& projection);

private:
    FireSimulator& fireSimulator;
    Shader fireShader;

    GLuint VAO, VBO, EBO, instanceBuffer, instanceColorVBO;
    void setupBuffers();
    void cleanupBuffers();
};

#endif // FIRERENDERER_H
