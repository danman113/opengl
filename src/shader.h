#pragma once
#include <iostream>
#include <glad/glad.h>
#include <glm/mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp>

#define OPENGL_ERROR_BUFFER_SIZE 255

struct Shader {
    std::string Source;
    unsigned int shaderId;
    unsigned int ShaderType;
    char errorBuffer[OPENGL_ERROR_BUFFER_SIZE] = {};
    Shader(std::string source, unsigned int shaderType);
    ~Shader();
};

struct ShaderProgram {
    std::unique_ptr<Shader> VertexShader;
    std::unique_ptr<Shader> FragmentShader;
    unsigned int programId = 0;
    char errorBuffer[OPENGL_ERROR_BUFFER_SIZE] = {};
    ShaderProgram(std::unique_ptr<Shader> vertexShader, std::unique_ptr<Shader> fragmentShader) : VertexShader(std::move(vertexShader)), FragmentShader(std::move(fragmentShader)) {
        std::cout << "Constructing program" << std::endl;
        programId = glCreateProgram();
        glAttachShader(programId, VertexShader->shaderId);
        glAttachShader(programId, FragmentShader->shaderId);
        glLinkProgram(programId);
        int success;
        glGetProgramiv(programId, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(programId, OPENGL_ERROR_BUFFER_SIZE, NULL, errorBuffer);
            std::cout << "Program compilation error: " << errorBuffer << std::endl;
        }
    }

    ShaderProgram* setUniform1f(const char* uniformLoc, float arg1) {
        int location = glGetUniformLocation(programId, uniformLoc);
        glUniform1f(location, arg1);
        return this;
    }

    ShaderProgram* setUniform2f(const char* uniformLoc, float arg1, float arg2) {
        int location = glGetUniformLocation(programId, uniformLoc);
        glUniform2f(location, arg1, arg2);
        return this;
    }

    ShaderProgram* setUniform3f(const char* uniformLoc, float arg1, float arg2, float arg3) {
        int location = glGetUniformLocation(programId, uniformLoc);
        glUniform3f(location, arg1, arg2, arg3);
        return this;
    }

    ShaderProgram* setUniform4f(const char* uniformLoc, float arg1, float arg2, float arg3, float arg4) {
        int location = glGetUniformLocation(programId, uniformLoc);
        glUniform4f(location, arg1, arg2, arg3, arg4);
        return this;
    }

    ShaderProgram* setUniformMat4(const char* uniformLoc, const glm::mat4& mat4) {
        int location = glGetUniformLocation(programId, uniformLoc);
        glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(mat4));
        return this;
    }

    ShaderProgram* use() {
        glUseProgram(programId);
        return this;
    }

    ~ShaderProgram() {
        std::cout << "Deleting program" << programId << std::endl;
        glDeleteProgram(programId);
    }
};