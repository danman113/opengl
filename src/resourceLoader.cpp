#include "resourceLoader.h"

SoundLoader::SoundLoader(shared_ptr<SoLoud::Soloud> soloudptr) : ResourceLoader(), soloud(soloudptr) {}
shared_ptr<SoLoud::Wav> SoundLoader::fetch(path p) {
	auto wav = std::make_shared<SoLoud::Wav>();
	auto pathStr = p.string();
	const char* rawPath = pathStr.c_str();
	wav->load(rawPath);
	return wav;
}


TextureLoader::TextureLoader() : ResourceLoader() {}
shared_ptr<ImageTexture> TextureLoader::fetch(path p) {
	auto texture = std::make_shared<ImageTexture>(p);
	texture->Init();
	return texture;
}


ShaderLoader::ShaderLoader() : SharedResourceMap() {}
shared_ptr<ShaderProgram> ShaderLoader::fetch(path vertexPath, path fragPath) {
	auto vertex = std::make_unique<Shader>(
		FS::readFile(vertexPath),
		GL_VERTEX_SHADER
		);
	auto fragment = std::make_unique<Shader>(
		FS::readFile(fragPath),
		GL_FRAGMENT_SHADER
		);

	auto shader = std::make_shared<ShaderProgram>(std::move(vertex), std::move(fragment));
	return shader;
}

void ShaderLoader::load(const vector<pair<pair<path, path>, string>>& assetList) {
	for (auto [paths, key] : assetList) {
		auto [vertex, fragment] = paths;
		map.insert({ key, fetch(vertex, fragment) });
	}
}
