#pragma once
#include <unordered_map>
#include <iostream>
#include <filesystem>
#include <string>
#include <memory>
#include <vector>
#include <utility>
#include <optional>

#include "soloud.h"
#include "soloud_wav.h"

#include "shader.h"
#include "fs.h"
#include "texture.h"


using std::string;
using std::unordered_map;
using std::vector;
using std::filesystem::path;
using std::shared_ptr;
using std::pair;
using std::optional;

template <typename ResourceType>
struct SharedResourceMap {
	unordered_map<string, shared_ptr<ResourceType>> map;
	optional<shared_ptr<ResourceType>> get(string key) {
		auto resource = map.find(key);
		if (resource != map.end()) {
			return resource->second;
		}
		return {};
	};
};

template <typename ResourceType>
class ResourceLoader: public SharedResourceMap<ResourceType> {
	virtual shared_ptr<ResourceType> fetch(path path) = 0;

public:
	ResourceLoader(): SharedResourceMap() {}

	void load(const vector<pair<path, string>>& assetList) {
		for (auto [path, key] : assetList) {
			map.insert({ key, fetch(path) });
		}
	};
};

class SoundLoader : public ResourceLoader<SoLoud::Wav> {
	shared_ptr<SoLoud::Soloud> soloud;
	shared_ptr<SoLoud::Wav> fetch (path p) override {
		auto wav = std::make_shared<SoLoud::Wav>();
		auto pathStr = p.string();
		const char* rawPath = pathStr.c_str();
		wav->load(rawPath);
		return wav;
	}
public:
	SoundLoader(shared_ptr<SoLoud::Soloud> soloudptr) : ResourceLoader(), soloud(soloudptr) {}
};

class TextureLoader : public ResourceLoader<Texture> {
	shared_ptr<Texture> fetch(path p) override {
		auto texture = std::make_shared<Texture>(p);
		return texture;
	}
public:
	TextureLoader() : ResourceLoader() {}
};

class ShaderLoader : public SharedResourceMap<ShaderProgram> {
	shared_ptr<ShaderProgram> fetch(path vertexPath, path fragPath) {
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
public:
	ShaderLoader() : SharedResourceMap() {}
	void load(const vector<pair<pair<path, path>, string>>& assetList) {
		for (auto [paths, key] : assetList) {
			auto [vertex, fragment] = paths;
			map.insert({ key, fetch(vertex, fragment) });
		}
	}
};
