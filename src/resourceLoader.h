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
	ResourceLoader::ResourceLoader() : SharedResourceMap() {}
	
	void ResourceLoader::load(const vector<pair<path, string>>& assetList) {
		for (auto [path, key] : assetList) {
			map.insert({ key, fetch(path) });
		}
	};
};

class SoundLoader : public ResourceLoader<SoLoud::Wav> {
	shared_ptr<SoLoud::Soloud> soloud;
	shared_ptr<SoLoud::Wav> fetch(path p) override;
public:
	SoundLoader(shared_ptr<SoLoud::Soloud> soloudptr);
};

class TextureLoader : public ResourceLoader<ImageTexture> {
	shared_ptr<ImageTexture> fetch(path p) override;
public:
	TextureLoader();
};

class ShaderLoader : public SharedResourceMap<ShaderProgram> {
	shared_ptr<ShaderProgram> fetch(path vertexPath, path fragPath);
public:
	ShaderLoader();
	void load(const vector<pair<pair<path, path>, string>>& assetList);
};
