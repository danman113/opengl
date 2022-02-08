#pragma once
#include <vector>
#include <filesystem>
#include <iostream>
#include <unordered_map>
#include <algorithm>
#include <stdexcept>
#include "fs.h"
#include "texture.h"

#include "stb_image_write.h"
#include "stb_rect_pack.h"
#include "stb_truetype.h"

struct CharacterCoordinates {
	int x1, y1, x2, y2;
	int kearningOffset;
	int width;
	int leftBearing;
};


class Font;
class BitmapTextRenderer {
	std::size_t bufferSize;
public:
	Font* font;
	unsigned int height;
	std::unordered_map<char, CharacterCoordinates> characterCache;
	std::string text;
	float scale;
	int ascent, descent, lineGap;
	int cursor = 0;
	BitmapTextRenderer(unsigned int _height, Font* _font, std::string _text);

	CharacterCoordinates& getBB(std::size_t i);

	std::size_t calculateBitmapWidth();

	void getCharSize(std::size_t i, CharacterCoordinates& BB);

	int getKearningSize(std::size_t i, CharacterCoordinates& BB);

	std::vector<unsigned char> writeToBitmap();

	void outImage();
};


using FontRange = std::pair<uint64_t, uint64_t>;
class FontAtlas {
	FontRange range;
	int bitmapWidth = 0xFF;
	int bitmapHeight = 0xFF;
	int padding = 1;
	int oversamplingRate = 2;
	stbtt_pack_context packContext;
	std::vector<stbtt_packedchar> characterData;
	Texture* texture = nullptr;
public:
	int size;
	Font* font;
	static FontRange GetRangeFromAlphabet(std::string const& alphabet) {
		uint64_t min = 0xFFFFFFFF;
		uint64_t max = 0;
		for (const auto& c : alphabet) {
			min = std::min<uint64_t> (c, min);
			max = std::max<uint64_t> (c, max);
		}
		return std::make_pair(min, max);
	};

	FontAtlas(Font* _font, int _size, FontRange f);

	std::vector<unsigned char> generateFont(std::filesystem::path p);

	Texture* generateTexture(std::filesystem::path p);

	stbtt_aligned_quad renderChar(uint64_t c, float* x, float* y);

	void outImage(std::filesystem::path p);
};

class Font {
	friend class BitmapTextRenderer;
	std::unordered_map<unsigned int, BitmapTextRenderer> fontMap;
public:
	stbtt_fontinfo info;
	std::vector<char> buffer;
	Font(std::filesystem::path p);

	void GenerateImage(unsigned int pixelSize, std::string text);
	std::vector<unsigned char> GenerateBitmap(unsigned int pixelSize, std::string text);
};