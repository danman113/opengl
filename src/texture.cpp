#include "texture.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


void Texture::Init(unsigned char* data) {
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);
    glPixelStorei(GL_UNPACK_ALIGNMENT, disableAliasing ? 1 : 4);
    glTexImage2D(GL_TEXTURE_2D, 0, colorSpace, width, height, 0, colorSpace, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void Texture::setActive() {
    glActiveTexture(textureId);
}

Texture::~Texture() {;
    glDeleteTextures(1, &textureId);
}

ImageTexture::ImageTexture(const std::filesystem::path& p) : path(p.string()) {}

ImageTexture::~ImageTexture() {
    std::cout << "Deleting texture " << path << std::endl;
}

void ImageTexture::Init() {
    stbi_set_flip_vertically_on_load(true);
    const char* pathStr = path.c_str();
    std::cout << "Loading image " << pathStr << std::endl;
    unsigned char* data = stbi_load(pathStr, &width, &height, &channels, 0);
    std::cout << "Texture" << width << " x " << height << "(" << &data << ")" << std::endl;
    if (!data) {
        std::cout << "failed to load image " << pathStr << std::endl;
        return;
    }
    Texture::Init(data);
    stbi_image_free(data);
}