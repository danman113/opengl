#include "mesh.h"

Mesh::Mesh(std::vector<float> geometry, std::shared_ptr<ShaderProgram> s) : Positions(geometry), shader(s) {
    std::cout << "Constructing mesh" << std::endl;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * Positions.size(), Positions.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, ATTRIB_SIZE, GL_FLOAT, GL_FALSE, ATTRIB_SIZE * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Mesh::draw() {
    shader->use();
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, Positions.size() / ATTRIB_SIZE);
}
Mesh::~Mesh() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}

TexturedMesh::TexturedMesh(std::vector<float> geometry, std::vector<float> uv, std::shared_ptr<ShaderProgram> s, std::shared_ptr<Texture> t) :
    Mesh(geometry, s), texture(t), UV(uv) {
    texture->Init();
    glGenVertexArrays(1, &textureVAO);
    glGenBuffers(1, &textureVBO);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, textureVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * UV.size(), UV.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(1, UV_SIZE, GL_FLOAT, GL_FALSE, sizeof(float) * UV_SIZE, (void*)0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 1);
    glBindVertexArray(1);
}

void TexturedMesh::draw() {
    shader->use();
    glBindTexture(GL_TEXTURE_2D, texture->textureId);
    glBindVertexArray(textureVAO);
    Mesh::draw();
}