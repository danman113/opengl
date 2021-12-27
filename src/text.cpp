#include "text.h"

#define STB_TRUETYPE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#include "stb_truetype.h"

static std::vector<char> buffer;
Font::Font(std::filesystem::path p) {
	buffer = FS::readFileAsBytes(p); // Reads the ttf file into a binary buffer
	const char* b = buffer.data();
	//FILE* fontFile = fopen(std::filesystem::absolute(p).string().c_str(), "rb");
	//fseek(fontFile, 0, SEEK_END);
	//std::size_t size = ftell(fontFile); /* how long is the file ? */
	//fseek(fontFile, 0, SEEK_SET); /* reset */

	//unsigned char* b = new unsigned char[size];

	//fread(b, size, 1, fontFile);
	//fclose(fontFile);
	// Inits the font data
	if (!stbtt_InitFont(&info, (const unsigned char*)b, 0)) {
		std::cout << "Failed to load font at " << p.string().c_str() << std::endl;
	}
}

void Font::GenerateImage(unsigned int pixelSize, std::string text) {
	FontFace f{ pixelSize, this, text };
	f.outImage();
}

FontFace::FontFace(unsigned int _height, Font* _font, std::string _text) : height(_height), text(_text), font(_font) {
	scale = stbtt_ScaleForPixelHeight(&font->info, height);
	std::cout << "Selected scale = " << scale << std::endl;

	stbtt_GetFontVMetrics(&font->info, &ascent, &descent, &lineGap);
	ascent = roundf(ascent * scale);
	descent = roundf(descent * scale);
}

CharacterCoordinates& FontFace::getBB(std::size_t i) {
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

std::size_t FontFace::calculateBitmapWidth() {
	std::size_t x = 0;
	for (int i = 0; i < text.length(); i++) {
		auto BoundingBox = getBB(i);
		x += BoundingBox.width + 1;
	}
	return x;
}

void FontFace::getCharSize(std::size_t i, CharacterCoordinates& BB) {
	int x = 0;
	auto c = text[i];
	stbtt_GetCodepointHMetrics(&font->info, c, &BB.width, &BB.leftBearing);
	stbtt_GetCodepointBitmapBox(&font->info, c, scale, scale, &BB.x1, &BB.y1, &BB.x2, &BB.y2);
	BB.width = roundf(BB.width * scale);
}

int FontFace::getKearningSize(std::size_t i, CharacterCoordinates& BB) {
	int kern = i == text.length() - 1 ? 0 : stbtt_GetCodepointKernAdvance(&font->info, text[i], text[i + 1]);
	BB.kearningOffset = roundf(kern * scale);
	return kern;
}

std::vector<unsigned char> FontFace::writeToBitmap() {
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

void FontFace::outImage() {
	auto buffer = writeToBitmap();
	stbi_write_png("out.png", buffer.capacity() / height, height, 1, buffer.data(), buffer.capacity() / height);
}

std::vector<unsigned char> Font::GenerateBitmap(unsigned int pixelSize, std::string text) {
	FontFace f{ pixelSize, this, text };
	return f.writeToBitmap();
}
