
#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

class Shader {

public:
    Shader(const std::string &vertexCode, const std::string &fragmentCode);
    ~Shader();

    // REVIEW: should these really be const? They aren't mutating the local application data 
    // (hence they're const as far as the compiler is concerned) but they are mutating state 
    // of the shader on the GPU
    void setUniformInt(const std::string &name, int value) const;
    void setUniformFloat(const std::string &name, float value) const;
    void setUniformVec2(const std::string &name, const glm::vec2 &value) const;
    void setUniformVec2(const std::string &name, float x, float y) const;
    void setUniformVec3(const std::string &name, const glm::vec3 &value) const;
    void setUniformVec3(const std::string &name, float x, float y, float z) const;
    void setUniformVec4(const std::string &name, const glm::vec4 &value) const;
    void setUniformVec4(const std::string &name, float x, float y, float z, float w) const;
    void setUniformMat3(const std::string &name, const glm::mat3 &matrix) const;
    void setUniformMat4(const std::string &name, const glm::mat4 &matrix) const;

    void setUniformBufferBindingPoint(const std::string &name, GLuint bindingPoint) const;

    void useShader();

    // prevent copy and copy-assignment
    Shader& operator=(const Shader&) = delete;
    Shader(const Shader&) = delete;

private:
    GLuint shaderProgramId;

    GLuint compileShader(const std::string &shaderCode, GLenum shaderType);

};

Shader::Shader(const std::string &vertexCode, const std::string &fragmentCode) {

    // ref: https://www.khronos.org/opengl/wiki/Shader_Compilation

    GLuint vertexId = compileShader(vertexCode, GL_VERTEX_SHADER);
    GLuint fragmentId = compileShader(fragmentCode, GL_FRAGMENT_SHADER);

    shaderProgramId = glCreateProgram();

    // attach shaders and link:
    glAttachShader(shaderProgramId, vertexId);
    glAttachShader(shaderProgramId, fragmentId);
    glLinkProgram(shaderProgramId);

    // check linking status:
    GLint result = 0;
    glGetProgramiv(shaderProgramId, GL_LINK_STATUS, &result);
    if (result == GL_FALSE) {
        GLint maxErrLength = 0;
        glGetProgramiv(shaderProgramId, GL_INFO_LOG_LENGTH, &maxErrLength);
        std::vector<GLchar> errorLog(maxErrLength);
        glGetProgramInfoLog(shaderProgramId, maxErrLength, &maxErrLength, &errorLog[0]);
        std::cout << "err linking shaders\n" << &errorLog[0] << std::endl;
        glDeleteProgram(shaderProgramId);
        glDeleteShader(vertexId);
        glDeleteShader(fragmentId);
        throw;
    }

    // delete the shaders now that they're linked:
    glDetachShader(shaderProgramId, vertexId);
    glDetachShader(shaderProgramId, fragmentId);
    glDeleteShader(vertexId);
    glDeleteShader(fragmentId);

}

Shader::~Shader() {
    glDeleteProgram(shaderProgramId);
}

void Shader::useShader() {
    glUseProgram(shaderProgramId);
}

void Shader::setUniformInt(const std::string &name, int value) const { 
    glUniform1i(glGetUniformLocation(shaderProgramId, name.c_str()), value);
}

void Shader::setUniformFloat(const std::string &name, float value) const { 
    glUniform1f(glGetUniformLocation(shaderProgramId, name.c_str()), value);
}

void Shader::setUniformVec2(const std::string &name, const glm::vec2 &value) const { 
    glUniform2fv(glGetUniformLocation(shaderProgramId, name.c_str()), 1, glm::value_ptr(value));
}

void Shader::setUniformVec2(const std::string &name, float x, float y) const { 
    glUniform2f(glGetUniformLocation(shaderProgramId, name.c_str()), x, y);
}

void Shader::setUniformVec3(const std::string &name, const glm::vec3 &value) const { 
    glUniform3fv(glGetUniformLocation(shaderProgramId, name.c_str()), 1, glm::value_ptr(value));
}

void Shader::setUniformVec3(const std::string &name, float x, float y, float z) const { 
    glUniform3f(glGetUniformLocation(shaderProgramId, name.c_str()), x, y, z);
}

void Shader::setUniformVec4(const std::string &name, const glm::vec4 &value) const { 
    glUniform4fv(glGetUniformLocation(shaderProgramId, name.c_str()), 1, glm::value_ptr(value));
}

void Shader::setUniformVec4(const std::string &name, float x, float y, float z, float w) const { 
    glUniform4f(glGetUniformLocation(shaderProgramId, name.c_str()), x, y, z, w);
}

void Shader::setUniformMat3(const std::string &name, const glm::mat3 &matrix) const {
    glUniformMatrix3fv(glGetUniformLocation(shaderProgramId, name.c_str()), 1, GL_FALSE, glm::value_ptr(matrix));
}

void Shader::setUniformMat4(const std::string &name, const glm::mat4 &matrix) const {
    glUniformMatrix4fv(glGetUniformLocation(shaderProgramId, name.c_str()), 1, GL_FALSE, glm::value_ptr(matrix));
}

void Shader::setUniformBufferBindingPoint(const std::string &name, GLuint bindingPoint) const {
    glUniformBlockBinding(shaderProgramId, glGetUniformBlockIndex(shaderProgramId, name.c_str()), bindingPoint);
}

GLuint Shader::compileShader(const std::string &shaderCode, GLenum shaderType) {

    GLuint shaderId = glCreateShader(shaderType);

    // glShaderSource takes an array of pointers to c-strings, but that's 
    // just a pointer to a c-string really:
    const char* code = shaderCode.c_str();
    glShaderSource(shaderId, 1, &code, nullptr);
    glCompileShader(shaderId);

    // check shader compilation:
    GLint result = 0;
    glGetShaderiv(shaderId, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE) {
        GLint maxErrLength = 0;
        glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &maxErrLength);
        std::vector<GLchar> errorLog(maxErrLength);
        glGetShaderInfoLog(shaderId, maxErrLength, &maxErrLength, &errorLog[0]);
        std::cout << "err compiling " << shaderType << " program\n" << &errorLog[0] << std::endl;
        glDeleteShader(shaderId);
        throw;
    }

    return shaderId;

}
