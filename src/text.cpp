#include "text.h"
#include <stdexcept>

#define STB_TRUETYPE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_RECT_PACK_IMPLEMENTATION
#include "stb_image_write.h"
#include "stb_truetype.h"
#include "stb_rect_pack.h"

Font::Font(std::filesystem::path p) {
	buffer = FS::readFileAsBytes(p); // Reads the ttf file into a binary buffer
	const char* b = buffer.data();
	// Inits the font data
	if (!stbtt_InitFont(&info, (const unsigned char*)b, 0)) {
		std::cout << "Failed to load font at " << p.string().c_str() << std::endl;
	}
}

void Font::GenerateImage(unsigned int pixelSize, std::string text) {
	BitmapTextRenderer f{ pixelSize, this, text };
	f.outImage();
}

FontAtlas::FontAtlas(Font* _font, int _size, FontRange f = GetRangeFromAlphabet(std::string("![a-zA-Z]~"))) : font(_font), size(_size), range(f), characterData(f.second - f.first) {}

std::vector<unsigned char> FontAtlas::generateFont(std::filesystem::path p) {
	auto fontBuffer = FS::readFileAsBytes(p); // Reads the ttf file into a binary buffer
	const char* b = fontBuffer.data();
	// Inits the font data
	stbtt_fontinfo info;
	if (!stbtt_InitFont(&info, (const unsigned char*)b, 0)) {
		std::cout << "Failed to load font at " << p.string().c_str() << std::endl;
	}
	while (bitmapHeight < (1<<15)) {
		stbtt_pack_context pack;
		std::vector<unsigned char> buffer (bitmapWidth * bitmapHeight);
		auto fontData = (const unsigned char*)b;
		stbtt_PackBegin(&pack, buffer.data(), bitmapWidth, bitmapHeight, 0, padding, 0);
		stbtt_PackSetOversampling(&pack, oversamplingRate, oversamplingRate);
		auto [min, max] = range;
		if (!stbtt_PackFontRange(&pack, fontData, 0, size, min, max - min, characterData.data())) {
			bitmapWidth *= 1.25;
			bitmapHeight *= 1.25;
			stbtt_PackEnd(&pack);
		}
		else {
			stbtt_PackEnd(&pack);
			return buffer;
		}
	}
	throw std::runtime_error("could not create texture");
}

Texture* FontAtlas::generateTexture(std::filesystem::path p) {
	texture = new Texture();
	auto fontBitmap = generateFont(p);
	texture->width = bitmapWidth;
	texture->height = bitmapHeight;
	texture->colorSpace = GL_RED;
	texture->disableAliasing = true;
	texture->Init(fontBitmap.data());
	return texture;
};

stbtt_aligned_quad FontAtlas::renderChar(uint64_t c, float* x, float* y) {
	if (texture == nullptr)
		throw std::runtime_error("Texture not generated");
	auto [min, max] = range;
	int index = c - min;
	stbtt_aligned_quad quad;
	stbtt_GetPackedQuad(characterData.data(), bitmapWidth, bitmapHeight, index, x, y, &quad, true);
	return quad;
};

void FontAtlas::outImage(std::filesystem::path p) {
	auto buffer = generateFont(p);
	stbi_write_png("out.png", bitmapHeight, bitmapWidth, 1, buffer.data(), bitmapWidth);
}

BitmapTextRenderer::BitmapTextRenderer(unsigned int _height, Font* _font, std::string _text) : height(_height), text(_text), font(_font) {
	scale = stbtt_ScaleForPixelHeight(&font->info, height);
	std::cout << "Selected scale = " << scale << std::endl;

	stbtt_GetFontVMetrics(&font->info, &ascent, &descent, &lineGap);
	ascent = roundf(ascent * scale);
	descent = roundf(descent * scale);
}

CharacterCoordinates& BitmapTextRenderer::getBB(std::size_t i) {
	CharacterCoordinates* BoundingBox = nullptr;
	auto c = text[i];
	if (auto it = characterCache.find(c); it != characterCache.end()) {
		return (it->second);
	}
	else {
		CharacterCoordinates BB;
		getCharSize(i, BB);
		getKearningSize(i, BB);
		characterCache.emplace(c, BB);
		it = characterCache.find(c);
		return (it->second);
	}
}

std::size_t BitmapTextRenderer::calculateBitmapWidth() {
	std::size_t x = 0;
	for (int i = 0; i < text.length(); i++) {
		auto BoundingBox = getBB(i);
		x += BoundingBox.width + 1;
	}
	return x;
}

void BitmapTextRenderer::getCharSize(std::size_t i, CharacterCoordinates& BB) {
	int x = 0;
	auto c = text[i];
	stbtt_GetCodepointHMetrics(&font->info, c, &BB.width, &BB.leftBearing);
	stbtt_GetCodepointBitmapBox(&font->info, c, scale, scale, &BB.x1, &BB.y1, &BB.x2, &BB.y2);
	BB.width = roundf(BB.width * scale);
}

int BitmapTextRenderer::getKearningSize(std::size_t i, CharacterCoordinates& BB) {
	int kern = i == text.length() - 1 ? 0 : stbtt_GetCodepointKernAdvance(&font->info, text[i], text[i + 1]);
	BB.kearningOffset = roundf(kern * scale);
	return kern;
}

std::vector<unsigned char> BitmapTextRenderer::writeToBitmap() {
	std::vector<unsigned char> imageBuffer(calculateBitmapWidth() * height);
	auto bufferWidth = (imageBuffer.capacity() / height);
	std::cout << "Made bitmap of size " << imageBuffer.capacity() << std::endl;
	for (int i = 0; i < text.length(); i++) {
		auto BB = getBB(i);
		/* get bounding box for character (may be offset to account for chars that dip above or below the line */
		int y = ascent + BB.y1;

		/* render character (stride and offset is important here) */
		int byteOffset = cursor + roundf(BB.leftBearing * scale) + (y * bufferWidth);
		auto width = (BB.x2 - BB.x1);
		stbtt_MakeCodepointBitmap(
			&font->info,
			imageBuffer.data() + byteOffset,
			width,
			BB.y2 - BB.y1,
			bufferWidth,
			scale,
			scale,
			text[i]
		);
		cursor += BB.width + BB.kearningOffset;
	}

	cursor = 0;
	return imageBuffer;
}

void BitmapTextRenderer::outImage() {
	auto buffer = writeToBitmap();
	stbi_write_png("out.png", buffer.capacity() / height, height, 1, buffer.data(), buffer.capacity() / height);
}

std::vector<unsigned char> Font::GenerateBitmap(unsigned int pixelSize, std::string text) {
	BitmapTextRenderer f{ pixelSize, this, text };
	return f.writeToBitmap();
}
