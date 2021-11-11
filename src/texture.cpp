#include "texture.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

void Texture::Init() {
    // glGenTextures(1, &textureId);
    // glBindTexture(GL_TEXTURE_2D, textureId);
    // glTexImage2D(textureId, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    // glGenerateMipmap(GL_TEXTURE_2D);

    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);
    // set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
}

Texture::Texture(std::filesystem::path path) {
    data = stbi_load(path.c_str(), &width, &height, &channels, 0);
    std::cout << "Texture" << width << " x " << height << "(" << &data << ")" << std::endl;
    if (!data)
        std::cout << "failed to load image " << path << std::endl;
    // std::cout << width << " x " << height << std::endl;
}

Texture::~Texture() {
    std::cout << "Deleting texture" << std::endl;
    stbi_image_free(data);
    glDeleteTextures(1, &textureId);
}