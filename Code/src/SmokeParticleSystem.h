#pragma once

#include <vector>
#include <glm/glm.hpp>

#include <GL/glew.h>
#include <GLFW/glfw3.h>


struct SmokeParticle {
    glm::vec3 position;
    glm::vec3 velocity;
    float age;
    float lifetime;
    glm::vec4 color;
    float size;
};

class SmokeParticleSystem {
public:
    SmokeParticleSystem();
    ~SmokeParticleSystem();

    void update(float deltaTime, const glm::vec3& windDirection, float windIntensity, float humidity);
    void render(const glm::mat4& view, const glm::mat4& projection);

    void emit(const glm::vec3& position);

    glm::vec3 windDirection;
    float windIntensity;
    float humidity;

private:
    std::vector<SmokeParticle> particles;

    // OpenGL specific members
    GLuint vao, vbo;
    GLuint shaderProgram;
    GLuint texture;

    void initGLResources();
    void releaseGLResources();
};