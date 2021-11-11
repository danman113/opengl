#pragma once
#include <vector>
#include <memory>
#include <iostream>
#include <glad/glad.h>
#include "texture.h"

#define OPENGL_ERROR_BUFFER_SIZE 255

struct Shader {
    std::string Source;
    unsigned int shaderId;
    unsigned int ShaderType;
    char errorBuffer[OPENGL_ERROR_BUFFER_SIZE];
    Shader(std::string source, unsigned int shaderType): Source(source), ShaderType(shaderType) {
        std::cout << "Constructing " << shaderType <<  " shader" << std::endl;
        shaderId = glCreateShader(ShaderType);
        const char* c = Source.c_str();
        glShaderSource(shaderId, 1, &c, NULL);
        glCompileShader(shaderId);
        int success;
        glGetShaderiv(shaderId, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shaderId, OPENGL_ERROR_BUFFER_SIZE, NULL, errorBuffer);
            std::cout << "Shader compilation error: " << errorBuffer << std::endl;
        }
    }
    ~Shader() {
        std::cout << "Deleting shader " << shaderId << std::endl;
        glDeleteShader(shaderId);
    }
};

struct ShaderProgram {
    std::unique_ptr<Shader> VertexShader;
    std::unique_ptr<Shader> FragmentShader;
    unsigned int programId;
    char errorBuffer[OPENGL_ERROR_BUFFER_SIZE];
    ShaderProgram(std::unique_ptr<Shader> vertexShader, std::unique_ptr<Shader> fragmentShader): VertexShader(std::move(vertexShader)), FragmentShader(std::move(fragmentShader)) {
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

    void setUniform1f(const char* uniformLoc, float arg1) {
        int location = glGetUniformLocation(programId, uniformLoc);
        glUniform1f(location, arg1);
    }

    void setUniform2f(const char* uniformLoc, float arg1, float arg2) {
        int location = glGetUniformLocation(programId, uniformLoc);
        glUniform2f(location, arg1, arg2);
    }

    void setUniform3f(const char* uniformLoc, float arg1, float arg2, float arg3) {
        int location = glGetUniformLocation(programId, uniformLoc);
        glUniform3f(location, arg1, arg2, arg3);
    }

    void setUniform4f(const char* uniformLoc, float arg1, float arg2, float arg3, float arg4) {
        int location = glGetUniformLocation(programId, uniformLoc);
        glUniform4f(location, arg1, arg2, arg3, arg4);
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

struct Mesh {
    std::vector<float> Positions;
    unsigned int VBO;
    unsigned int VAO;
    static const int ATTRIB_SIZE = 3;
    std::unique_ptr<ShaderProgram> shader;
    Mesh(std::vector<float> geometry, std::unique_ptr<ShaderProgram> s): Positions(geometry), shader(std::move(s)) {
        std::cout << "Constructing mesh" << std::endl;
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * Positions.size(), Positions.data(), GL_STATIC_DRAW);
        
        glVertexAttribPointer(0, ATTRIB_SIZE, GL_FLOAT, GL_FALSE,  ATTRIB_SIZE * sizeof(float), (void*) 0);
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, 0); 
        glBindVertexArray(0); 
    }

    virtual void draw() {
        shader->use();
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, Positions.size() / ATTRIB_SIZE);
    }
    ~Mesh() {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
    }
};

struct TexturedMesh : public Mesh {
    Texture* texture;
    std::vector<float> UV;
    unsigned int textureVBO;
    unsigned int textureVAO;
    static const int UV_SIZE = 2;
    TexturedMesh(std::vector<float> geometry, std::vector<float> uv, std::unique_ptr<ShaderProgram> s, Texture* t):
        Mesh(geometry, std::move(s)), texture(t), UV(uv) {
            texture->Init();
            glGenVertexArrays(1, &textureVAO);
            glGenBuffers(1, &textureVBO);
            glBindVertexArray(VAO);

            glBindBuffer(GL_ARRAY_BUFFER, textureVBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(float) * UV.size(), UV.data(), GL_STATIC_DRAW);

            glVertexAttribPointer(1, UV_SIZE, GL_FLOAT, GL_FALSE, sizeof(float) * UV_SIZE, (void*) 0);
            glEnableVertexAttribArray(1);

            glBindBuffer(GL_ARRAY_BUFFER, 1);
            glBindVertexArray(1);
        }
    virtual void draw() {
        shader->use();
        glBindTexture(GL_TEXTURE_2D, texture->textureId);
        glBindVertexArray(textureVAO);
        Mesh::draw();
    }

    ~TexturedMesh() {
        delete texture;
    }
};
class Renderer {
    
};
