#pragma once
#include <vector>
#include <memory>
#include <iostream>
#include <glad/glad.h>
#define OPENGL_ERROR_BUFFER_SIZE 255

struct Shader {
    std::string Source;
    unsigned int shaderId;
    unsigned int ShaderType;
    char errorBuffer[OPENGL_ERROR_BUFFER_SIZE];
    Shader(std::string source, unsigned int shaderType): Source(source), ShaderType(shaderType) {
        std::cout << "Constructing " << shaderType <<  " shader" << std::endl;
        shaderId = glCreateShader(ShaderType);
        const char * c = Source.c_str();
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
    ~ShaderProgram() {
        std::cout << "Deleting program" << programId << std::endl;
        glDeleteProgram(programId);
    }
};



struct Mesh {
    std::vector<float> Positions;
    unsigned int VBO;
    unsigned int VAO;
    std::unique_ptr<ShaderProgram> shader;
    Mesh(std::vector<float> geometry, std::unique_ptr<ShaderProgram> s): Positions(geometry), shader(std::move(s)) {
        std::cout << "Constructing mesh" << std::endl;
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glBindVertexArray(VAO);

        
        std::cout << "Building mesh of size " << Positions.size() << "(" << sizeof(float) * Positions.size() << ")" << std::endl;
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * Positions.size(), Positions.data(), GL_STATIC_DRAW);
        
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, (Positions.size() / 3) * sizeof(float), (void*) 0);
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, 0); 
        glBindVertexArray(0); 
    }

    virtual void draw() {
        glUseProgram(shader->programId);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);
    }
    ~Mesh() {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
    }
};

class Renderer {
    
};
