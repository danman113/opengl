#include "shader.h"

Shader::Shader(std::string source, unsigned int shaderType) : Source(source), ShaderType(shaderType) {
    std::cout << "Constructing " << shaderType << " shader" << std::endl;
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
Shader::~Shader() {
    std::cout << "Deleting shader " << shaderId << std::endl;
    glDeleteShader(shaderId);
}