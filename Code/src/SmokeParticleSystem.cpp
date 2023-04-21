#include "SmokeParticleSystem.h"
#include <algorithm>
#include <random>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "SmokeParticleShader.h"
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

int MAX_PARTICLES = 1000000;

SmokeParticleSystem::SmokeParticleSystem() {
    initGLResources();
}

SmokeParticleSystem::~SmokeParticleSystem() {
    releaseGLResources();
}

void SmokeParticleSystem::update(float deltaTime, const glm::vec3& windDirection, float windIntensity, float humidity) {

    float startSize = 100.0f;
    float endSize = 10.0f;
    float startAlpha = 0.8f;
    float endAlpha = 0.0f;

    // Update particles
    for (auto& particle : particles) {
        // Compute simple sine-based oscillation for movement
        float oscillationFrequency = 0.1f; // Adjust the frequency of the oscillation
        float oscillationIntensity = 0.5f; // Adjust the intensity of the oscillation
        glm::vec3 oscillationForce = glm::vec3(
            oscillationIntensity * sin(deltaTime * oscillationFrequency),
            0.0f,
            oscillationIntensity * sin(deltaTime * oscillationFrequency)
        );

        // Update particle velocity with oscillation force
        particle.velocity += oscillationForce * deltaTime;

        // Add wind influence to particle velocity
        particle.velocity += windDirection * windIntensity * deltaTime;

        // Dampen particle velocity based on humidity
        particle.velocity *= (1.0f - humidity * deltaTime);

        // Update particle position and age
        particle.position += particle.velocity * deltaTime;
        particle.age += deltaTime;

        float lifeRatio = particle.age / particle.lifetime;
        particle.size = glm::mix(startSize, endSize, lifeRatio);
        particle.color.a = glm::mix(startAlpha, endAlpha, lifeRatio);
    }

    // Remove dead particles
    particles.erase(std::remove_if(particles.begin(), particles.end(), [](const SmokeParticle& particle) {
        return particle.age >= particle.lifetime;
    }), particles.end());
}

void SmokeParticleSystem::render(const glm::mat4& view, const glm::mat4& projection) {
    // Set up OpenGL state for rendering particles
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glUseProgram(shaderProgram);
    glBindVertexArray(vao);

    // Set view and projection matrix uniforms
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    // Update the VBO with the current particle data
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(SmokeParticle) * particles.size(), particles.data(), GL_DYNAMIC_DRAW);

    // Draw the particles as points
    glDrawArrays(GL_POINTS, 0, particles.size()); 

    // Reset OpenGL state
    glBindVertexArray(0);
    glUseProgram(0);
    glDisable(GL_BLEND);
}



void SmokeParticleSystem::emit(const glm::vec3& position) {
    SmokeParticle particle;
    particle.position = position;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::normal_distribution<float> dis(0.0f, 0.3f);

    float x = dis(gen);
    float z = dis(gen);
    float y = std::abs(dis(gen)) + 1.0f;

    glm::vec3 randomVelocity(x, y, z);
    particle.velocity = randomVelocity;
    particle.velocity = randomVelocity;
    // particle.velocity = velocity;

    particle.age = 0.0f;
    particle.lifetime = 6.0f; // Adjust this value as needed
    
    
    std::uniform_real_distribution<float> colorDis(0.1f, 0.2f);
    std::uniform_real_distribution<float> alphaDis(0.1f, 0.4f);
    particle.color = glm::vec4(colorDis(gen), colorDis(gen), colorDis(gen), alphaDis(gen));

    //particle.color = glm::vec4(0.5f, 0.5f, 0.5f, 0.5f); // Adjust this value as needed
    
    std::uniform_real_distribution<float> sizeDis(10.0f, 200.0f);
    particle.size = sizeDis(gen);
    // particle.size = 1.0f; // Adjust this value as needed

    particles.push_back(particle);
}

void SmokeParticleSystem::initGLResources() {
    // Initialize OpenGL resources (e.g., VAO, VBO, shaders, and texture)

    // Load shaders and create a shader program
    GLuint vertexShader = create_shader(GL_VERTEX_SHADER, "../resources/shaders/smoke_shader.vert");
    GLuint fragmentShader = create_shader(GL_FRAGMENT_SHADER, "../resources/shaders/smoke_shader.frag");
    shaderProgram = create_shader_program(vertexShader, fragmentShader);

    // Create VAO and VBO
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(SmokeParticle) * MAX_PARTICLES, nullptr, GL_DYNAMIC_DRAW);

    // Set up vertex attribute pointers
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(SmokeParticle), (GLvoid*)offsetof(SmokeParticle, position));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, sizeof(SmokeParticle), (GLvoid*)offsetof(SmokeParticle, age));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(SmokeParticle), (GLvoid*)offsetof(SmokeParticle, lifetime));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(SmokeParticle), (GLvoid*)offsetof(SmokeParticle, color));
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, sizeof(SmokeParticle), (GLvoid*)offsetof(SmokeParticle, size));
    glEnableVertexAttribArray(4);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

}

void SmokeParticleSystem::releaseGLResources() {
    // Release OpenGL resources (e.g., VAO, VBO, shaders, and texture)

    // Delete VAO and VBO
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);

    // Delete shader program
    glDeleteProgram(shaderProgram);

    // Delete texture
    glDeleteTextures(1, &texture);
}