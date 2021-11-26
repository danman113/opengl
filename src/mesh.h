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
    static const int ATTRIB_SIZE = 3;
    std::shared_ptr<ShaderProgram> shader;
    Mesh(std::vector<float> geometry, std::shared_ptr<ShaderProgram> s);

    virtual void draw();
    ~Mesh();
};

struct TexturedMesh : public Mesh {
    std::shared_ptr<Texture> texture;
    std::vector<float> UV;
    unsigned int textureVBO;
    unsigned int textureVAO;
    static const int UV_SIZE = 2;
    TexturedMesh(std::vector<float> geometry, std::vector<float> uv, std::shared_ptr<ShaderProgram> s, std::shared_ptr<Texture> t);
    virtual void draw();
};