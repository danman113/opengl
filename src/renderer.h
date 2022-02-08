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
#include "text.h"

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

struct TextLine {
    RenderableId id;
    std::string text;
    glm::mat4 transform;
    std::vector<std::vector<float>> UVs;
    std::vector<std::vector<float>> Positions;
    std::shared_ptr<FontAtlas> font;
    TextLine(RenderableId _id, std::string _text, glm::mat4 _transform, std::shared_ptr<FontAtlas> f):
        id(_id), text(_text), transform(_transform), font(f), UVs(), Positions() {
        UVs.reserve(_text.length());
        Positions.reserve(_text.length());
        float x = 0, y = 0;
        float lastX = 0;
        for (auto& ch : _text) {
            auto glyphData = font->renderChar(ch, &x, &y);
            UVs.push_back({
                glyphData.s1, glyphData.t1,
                glyphData.s1, glyphData.t0,
                glyphData.s0, glyphData.t1,
                // second triangle
                glyphData.s1, glyphData.t0,
                glyphData.s0, glyphData.t0,
                glyphData.s0, glyphData.t1
            });
            float divisor = font->size;

            Positions.push_back({
                glyphData.x1 / divisor, glyphData.y1 / divisor,
                glyphData.x1 / divisor, glyphData.y0 / divisor,
                glyphData.x0 / divisor, glyphData.y1 / divisor,
                // second triangle
                glyphData.x1 / divisor, glyphData.y0 / divisor,
                glyphData.x0 / divisor, glyphData.y0 / divisor,
                glyphData.x0 / divisor, glyphData.y1 / divisor
            });

        }
    }
};

struct TextRenderer : public Renderer<TextLine> {
    std::unique_ptr<TexturedMesh> characterMesh;
    std::shared_ptr<FontAtlas> font;
    std::shared_ptr<PerspectiveCamera> camera;
    TextRenderer(std::unique_ptr<TexturedMesh> m, std::shared_ptr<FontAtlas> f, std::shared_ptr<PerspectiveCamera> c) : characterMesh(std::move(m)), font(f), camera(c) {}
    
    virtual void DrawEntity(const TextLine& textLine) {
        auto zIndex = textLine.transform[3][2];
        auto pos = glm::mat4(textLine.transform);
        float x = 0, y = 0;
        float lastX = 0;
        for (int i = 0; i < textLine.text.length(); ++i) {
            auto& ch = textLine.text[i];
            auto glyphData = font->renderChar(ch, &x, &y);
            auto a = textLine.UVs[i];
            characterMesh->UV = a;
            characterMesh->updateUVs();
            auto b = textLine.Positions[i];
            characterMesh->Positions = b;
            characterMesh->updatePositions();
            characterMesh->shader->use()
                ->setUniform1f("zIndex", zIndex)
                ->setUniformMat4("model", pos);
            pos = glm::translate(pos, glm::vec3(0.0f, 0.0f, 1.0f));
            camera->applyToShader(*(characterMesh->shader));
            characterMesh->draw();
        }
    }
    template<typename ... Ts>
    void add(Ts ... args) {
        entities.push_back(TextLine(args..., font));
    }
};


struct TypeWriterRenderer : public TextRenderer {
    int frames = 0;
    int length = 0;
    TypeWriterRenderer(std::unique_ptr<TexturedMesh> m, std::shared_ptr<FontAtlas> f, std::shared_ptr<PerspectiveCamera> c):
        TextRenderer(std::move(m), f, c) {}
    virtual void DrawEntity(const TextLine& textLine) {
        auto zIndex = textLine.transform[3][2];
        auto pos = glm::mat4(textLine.transform);
        float x = 0, y = 0;
        float lastX = 0;
        // Todo: replace this with something sensible
        frames++;
        if (frames < 0)
            frames = 0;
        if (frames % 60 == 0) {
            length++;
        }
        int maxSize = textLine.text.length();
        for (int i = 0; i < maxSize && i < (length % maxSize); ++i) {
            auto& ch = textLine.text[i];
            auto glyphData = font->renderChar(ch, &x, &y);
            auto a = textLine.UVs[i];
            characterMesh->UV = a;
            characterMesh->updateUVs();
            auto b = textLine.Positions[i];
            characterMesh->Positions = b;
            characterMesh->updatePositions();
            characterMesh->shader->use()
                ->setUniform1f("zIndex", zIndex)
                ->setUniformMat4("model", pos);
            pos = glm::translate(pos, glm::vec3(0.0f, 0.0f, 1.0f));
            camera->applyToShader(*(characterMesh->shader));
            characterMesh->draw();
        }
    }
};