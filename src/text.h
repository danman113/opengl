#pragma once
#include <vector>
#include <filesystem>
#include <iostream>
#include <unordered_map>
#include "fs.h"

#include "stb_image_write.h"
#include "stb_truetype.h"

struct CharacterCoordinates {
	int x1, y1, x2, y2;
	int kearningOffset;
	int width;
	int leftBearing;
};


class Font;
class FontFace {
	std::size_t bufferSize;
public:
	Font* font;
	unsigned int height;
	std::unordered_map<char, CharacterCoordinates> characterCache;
	std::string text;
	float scale;
	int ascent, descent, lineGap;
	int cursor = 0;
	FontFace(unsigned int _height, Font* _font, std::string _text);

	CharacterCoordinates& getBB(std::size_t i);

	std::size_t calculateBitmapWidth();

	void getCharSize(std::size_t i, CharacterCoordinates& BB);

	int getKearningSize(std::size_t i, CharacterCoordinates& BB);

	std::vector<unsigned char> writeToBitmap();

	void outImage();
};

class Font {
	friend class FontFace;
	std::unordered_map<unsigned int, FontFace> fontMap;
	stbtt_fontinfo info;
public:
	Font(std::filesystem::path p);

	void GenerateImage(unsigned int pixelSize, std::string text);
	std::vector<unsigned char> GenerateBitmap(unsigned int pixelSize, std::string text);
};