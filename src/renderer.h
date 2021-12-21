#pragma once
#include <cstdint>
#include <exception>
#include <vector>
#include <iostream>
#include <memory>

#include <glm/mat4x4.hpp>

#include "texture.h"
#include "mesh.h"
#include "camera.h"

using RenderableId = std::uint64_t;

template <typename EntityType>
struct Renderer {
    std::vector<EntityType> entities;
    virtual void DrawEntity(const EntityType& e) = 0;
    void Render() {
        for (auto& entity : entities) {
            DrawEntity(entity);
        }
    }
    template<typename ... Ts>
    void add(Ts ... args) {
        entities.push_back(EntityType(args...));
    }
};


struct Sprite {
    RenderableId id;
    std::shared_ptr<Texture> texture;
    glm::mat4 transform;

    Sprite(RenderableId _id, std::shared_ptr<Texture> _texture, glm::mat4 _transform):
        id(_id), texture(_texture), transform(_transform) {}
};

struct SpriteRenderer : public Renderer<Sprite> {
    std::unique_ptr<TexturedMesh> mesh;
    std::shared_ptr<PerspectiveCamera> camera;
    SpriteRenderer(std::unique_ptr<TexturedMesh> m, std::shared_ptr<PerspectiveCamera> c): mesh(std::move(m)), camera(c) {}
    virtual void DrawEntity(const Sprite& sprite) {
        mesh->setTexture(sprite.texture);
        auto zIndex = sprite.transform[3][2];
        mesh->shader->use()->setUniform1f("zIndex", zIndex)->setUniformMat4("model", sprite.transform);
        camera->applyToShader(*(mesh->shader));
        mesh->draw();
    }
};
