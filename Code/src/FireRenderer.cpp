#include "FireRenderer.h"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

FireRenderer::FireRenderer(FireSimulator& fireSimulator)
    : fireSimulator(fireSimulator),
      fireShader("../resources/shaders/fire_shader.vert", "../resources/shaders/fire_shader.frag") {
    setupBuffers();
}

FireRenderer::~FireRenderer() {
    cleanupBuffers();
}

void FireRenderer::render(const glm::mat4& view, const glm::mat4& projection) {
    fireShader.use();
    glBindVertexArray(VAO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

    for (int z = 0; z < fireSimulator.voxelGrid.depth; ++z) {
        for (int y = 0; y < fireSimulator.voxelGrid.height; ++y) {
            for (int x = 0; x < fireSimulator.voxelGrid.width; ++x) {
                glm::ivec3 pos(x, y, z);
                Voxel& voxel = fireSimulator.getVoxel(pos);

                if (voxel.state != EMPTY) {
                    glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, z));
                    fireShader.setMat4("model", model);
                    fireShader.setMat4("view", view);
                    fireShader.setMat4("projection", projection);
                    fireShader.setVec3("voxelColor", voxel.color);

                    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
                    GLenum error = glGetError();
                    if (error != GL_NO_ERROR) {
                        std::cout << "OpenGL error: " << error << std::endl;
                    }
                }
            }
        }
    }
}

void FireRenderer::setupBuffers() {
    float vertices[] = {
        // positions
        -0.5f, -0.5f, -0.5f,
        0.5f, -0.5f, -0.5f,
        0.5f,  0.5f, -0.5f,
        0.5f,  0.5f, -0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,

        -0.5f, -0.5f,  0.5f,
        0.5f, -0.5f,  0.5f,
        0.5f,  0.5f,  0.5f,
        0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f,

        -0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,

        0.5f,  0.5f,  0.5f,
        0.5f,  0.5f, -0.5f,
        0.5f, -0.5f, -0.5f,
        0.5f, -0.5f, -0.5f,
        0.5f, -0.5f,  0.5f,
        0.5f,  0.5f,  0.5f,

        -0.5f, -0.5f, -0.5f,
        0.5f, -0.5f, -0.5f,
        0.5f, -0.5f,  0.5f,
        0.5f, -0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f,
        -0.5f, -0.5f, -0.5f,

        -0.5f,  0.5f, -0.5f,
        0.5f,  0.5f, -0.5f,
        0.5f,  0.5f,  0.5f,
        0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f, -0.5f
    };

    GLuint indices[] = {
        0, 1, 2,
        2, 3, 0,
        4, 5, 6,
        6, 7, 4,
        8, 9, 10,
        10, 11, 8,
        12, 13, 14,
        14, 15, 12,
        16, 17, 18,
        18, 19, 16,
        20, 21, 22,
        22, 23, 24
    };

    // Generate and bind VAO
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // Generate and bind VBO
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Generate and bind EBO
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Set vertex attribute pointers
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Unbind VAO, VBO and EBO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

}

void FireRenderer::cleanupBuffers() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}