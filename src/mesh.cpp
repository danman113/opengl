#include "mesh.h"

Mesh::Mesh(std::vector<float> geometry, std::shared_ptr<ShaderProgram> s, int attrib_size = 3, int drawType = GL_STATIC_DRAW) : Positions(geometry), shader(s), ATTRIB_SIZE(attrib_size) {
    std::cout << "Constructing mesh" << std::endl;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * Positions.size(), Positions.data(), drawType);
    glVertexAttribPointer(0, ATTRIB_SIZE, GL_FLOAT, GL_FALSE, ATTRIB_SIZE * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Mesh::updatePositions() {
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * Positions.size(), Positions.data());
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

Shape::Shape(std::vector<float> geometry, std::shared_ptr<ShaderProgram> s, int drawType = GL_STATIC_DRAW) : Mesh(geometry, s, 2, drawType) {}


TexturedMesh::TexturedMesh(
    std::vector<float> geometry,
    std::vector<float> uv,
    std::shared_ptr<ShaderProgram> s,
    std::shared_ptr<Texture> t,
    int drawType = GL_STATIC_DRAW
) : Shape(geometry, s, drawType), texture(t), UV(uv) {
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

void TexturedMesh::updateUVs() {
    glBindBuffer(GL_ARRAY_BUFFER, textureVBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * UV.size(), UV.data());
}

void TexturedMesh::draw() {
    shader->use();
    texture->setActive();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture->textureId);
    glBindVertexArray(textureVAO);
    Mesh::draw();
}

void TexturedMesh::setTexture(std::shared_ptr<Texture> t)
{
    texture = t;
}

