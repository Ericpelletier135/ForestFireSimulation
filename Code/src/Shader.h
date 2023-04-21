#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>

class Shader {
public:
    Shader(const std::string& vertexPath, const std::string& fragmentPath);
    ~Shader();

    void use() const;
    void setVec3(const std::string& name, const glm::vec3& value) const;
    void setMat4(const std::string& name, const glm::mat4& value) const;
    void setVec3Array(const std::string& name, const std::vector<glm::vec3>& values) const;

private:
    GLuint ID;
    void checkCompileErrors(GLuint shader, const std::string& type) const;
};

#endif
