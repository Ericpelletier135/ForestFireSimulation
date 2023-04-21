#include <fstream>
#include <sstream>
#include <iostream>
#include <GL/glew.h>
#include <vector>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

GLuint create_shader(GLenum type, const char* file_path) {
    // Read shader source from file
    std::ifstream shader_file(file_path);
    if (!shader_file.is_open()) {
        std::cerr << "Failed to open shader file: " << file_path << std::endl;
        return 0;
    }

    std::stringstream shader_stream;
    shader_stream << shader_file.rdbuf();
    shader_file.close();

    std::string shader_str = shader_stream.str();
    const char* shader_src = shader_str.c_str();

    // Compile shader
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &shader_src, nullptr);
    glCompileShader(shader);

    // Check for compile errors
    GLint compile_status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compile_status);
    if (!compile_status) {
        GLint info_log_length;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &info_log_length);

        std::vector<char> info_log(info_log_length);
        glGetShaderInfoLog(shader, info_log_length, nullptr, info_log.data());

        std::cerr << "Failed to compile shader: " << file_path << std::endl;
        std::cerr << info_log.data() << std::endl;

        glDeleteShader(shader);
        return 0;
    }

    return shader;
}

GLuint create_shader_program(GLuint vertex_shader, GLuint fragment_shader) {
    // Create shader program and attach shaders
    GLuint program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);

    // Link shader program
    glLinkProgram(program);

    // Check for linking errors
    GLint link_status;
    glGetProgramiv(program, GL_LINK_STATUS, &link_status);
    if (!link_status) {
        GLint info_log_length;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &info_log_length);

        std::vector<char> info_log(info_log_length);
        glGetProgramInfoLog(program, info_log_length, nullptr, info_log.data());

        std::cerr << "Failed to link shader program" << std::endl;
        std::cerr << info_log.data() << std::endl;

        glDeleteProgram(program);
        return 0;
    }

    // Detach and delete shaders after linking
    glDetachShader(program, vertex_shader);
    glDetachShader(program, fragment_shader);
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    return program;
}

GLuint loadTexture(const std::string& filepath) {
    int width, height, numComponents;
    unsigned char* data = stbi_load(filepath.c_str(), &width, &height, &numComponents, 0);
    if (!data) {
        std::cerr << "Failed to load texture: " << filepath << std::endl;
        return 0;
    }

    GLenum format;
    if (numComponents == 1) {
        format = GL_RED;
    } else if (numComponents == 3) {
        format = GL_RGB;
    } else if (numComponents == 4) {
        format = GL_RGBA;
    } else {
        std::cerr << "Unsupported number of components in texture: " << filepath << std::endl;
        stbi_image_free(data);
        return 0;
    }

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_2D, 0);
    stbi_image_free(data);

    return texture;
}