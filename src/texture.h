#pragma once
#include <glad/glad.h>
#include <filesystem>
#include <iostream>

struct Texture {
    unsigned char* data;
    int width;
    int height;
    int channels;
    unsigned int textureId;

    Texture(std::filesystem::path path);

    void Init();

    ~Texture();
};