// Copyright © 2026 by Evan O'Bryant
// Do not download, use, look at, skim, or utilize this code in any sense for the purposes of training/aiding/improving any form of Artificial Intelligence including LLMs.
#pragma once

#include <unordered_map>
#include <filesystem>
#include <concepts>
#include <string_view>
#include <optional>

#include <raylib.h>

template <typename T, typename... ValidTypes>
concept IsAnyOf = (std::same_as<T, ValidTypes> || ...);

struct ModelAnimationInfo {
	ModelAnimation* pData = nullptr;
	int count = 0;

	int curFrame = 0;
	float curFrameTime = 0;
	float fps = 1.f / 60;
	float speedMult = 1;
};

struct MaterialInfo {
	Material* pData = nullptr;
	int count = 0;
};

template <typename T>
concept RayLibAsset = IsAnyOf<T,
	Texture,
	Font,
	Model,
	ModelAnimationInfo,
	MaterialInfo,
	Sound,
	Music
>;

class AssetManager
{
public:
	static AssetManager& get();

	AssetManager() = default;
	~AssetManager();

	AssetManager(const AssetManager&) = delete;
	AssetManager& operator=(const AssetManager&) = delete;

	void load_all(const std::filesystem::path& baseDirectory,
				  const std::optional<std::filesystem::path> musicDirectory = {});

	template <std::convertible_to<std::filesystem::path>... AssetPaths>
	void load(AssetPaths... nonMusicPaths);

	// you have to load music separately im sorry
	template <std::convertible_to<std::filesystem::path>... AssetPaths>
	void load_music(AssetPaths... musicPaths);

	template <RayLibAsset AssetType>
	auto& get(std::string_view assetName);

	template <RayLibAsset AssetType>
	std::size_t get_index(std::string_view assetName);

	template <RayLibAsset AssetType>
	auto& get_from_index(std::size_t index);

	void unload_all();

private:
	void Load(const std::filesystem::path& path);
	void LoadMusic(const std::filesystem::path& path);

	// courtesy of nathan braggs + cppreference
	// allows for lookup with string_view without allocating a string
	struct StringHash {
		using is_transparent = void;
		std::size_t operator()(const char* cstr) const {
			return std::hash<std::string_view>{}(cstr);
		}

		std::size_t operator()(std::string_view sv) const {
			return std::hash<std::string_view>{}(sv);
		}

		std::size_t operator()(const std::string& str) const {
			return std::hash<std::string_view>{}(str);
		}
	};

	using AssetMap = std::unordered_map<std::string, std::size_t, StringHash, std::equal_to<>>;
	
	template <typename T>
	using AssetVec = std::vector<T>;

#define ASSET_COLLECTION(Type, Name) \
	AssetMap m_##Name##Map{}; \
	AssetVec<Type> m_##Name##Vec{}

	ASSET_COLLECTION(Texture, Textures);
	ASSET_COLLECTION(Font, Fonts);
	ASSET_COLLECTION(Model, Models);
	ASSET_COLLECTION(ModelAnimationInfo, ModelAnims);
	ASSET_COLLECTION(MaterialInfo, Materials);
	ASSET_COLLECTION(Sound, Sounds);
	ASSET_COLLECTION(Music, Music);

#undef ASSET_COLLECTION
};

#include "AssetManager.cxx"
