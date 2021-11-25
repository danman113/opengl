#pragma once
#include <glad/glad.h>
#include <filesystem>
#include <iostream>

struct Texture {
    unsigned char* data;
    int width;
    int height;
    int channels;
    std::string path;
    unsigned int textureId;

    Texture(const std::filesystem::path& path);

    void Init();
    void setActive();

    ~Texture();
};