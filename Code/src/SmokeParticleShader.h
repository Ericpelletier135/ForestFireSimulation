#pragma once

#include <GL/glew.h>

GLuint create_shader(GLenum type, const char* file_path);
GLuint create_shader_program(GLuint vertex_shader, GLuint fragment_shader);
GLuint loadTexture(const std::string& filepath);