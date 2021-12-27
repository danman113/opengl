#pragma once
#include <glad/glad.h>
#include <filesystem>
#include <iostream>

struct Texture {
    int width;
    int height;
    unsigned int textureId;
    int colorSpace = GL_RGB;
    bool disableAliasing = false;

    void Init(unsigned char* data);
    void setActive();

    ~Texture();
};

struct ImageTexture : public Texture {
    int channels;
    std::string path;
    ImageTexture(const std::filesystem::path& path);
    void Init();
    ~ImageTexture();
};

