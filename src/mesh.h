#pragma once
#include <vector>
#include <memory>
#include <iostream>
#include <glad/glad.h>
#include "texture.h"
#include "shader.h"

struct Mesh {
    std::vector<float> Positions;
    unsigned int VBO;
    unsigned int VAO;
    int ATTRIB_SIZE;
    std::shared_ptr<ShaderProgram> shader;
    Mesh(std::vector<float> geometry, std::shared_ptr<ShaderProgram> s, int attrib_size, int drawType);

    virtual void draw();
    ~Mesh();
};

struct Shape : public Mesh {
    Shape(std::vector<float> geometry, std::shared_ptr<ShaderProgram> s);
};

struct TexturedMesh : public Shape {
    std::shared_ptr<Texture> texture;
    std::vector<float> UV;
    unsigned int textureVBO;
    unsigned int textureVAO;
    static const int UV_SIZE = 2;
    TexturedMesh(std::vector<float> geometry, std::vector<float> uv, std::shared_ptr<ShaderProgram> s, std::shared_ptr<Texture> t);
    void setTexture(std::shared_ptr<Texture> t);
    virtual void draw();
};