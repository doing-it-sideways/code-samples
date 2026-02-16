#include "AssetManager.hpp"

#include <ranges>
#include <cctype>
#include <cassert>
#include <iostream>

AssetManager& AssetManager::get() {
	static AssetManager instance;
	return instance;
}

AssetManager::~AssetManager() {
	unload_all();
}

namespace fs = std::filesystem;

void AssetManager::load_all(const fs::path& baseDirectory,
							const std::optional<fs::path> musicDirectory)
{
	fs::directory_entry directory = fs::directory_entry{ baseDirectory };
	assert(directory.exists() && directory.is_directory());

	for (auto it = fs::recursive_directory_iterator{ baseDirectory }; it != fs::recursive_directory_iterator{}; ++it) {
		if (it->is_directory()) {
			// skip music directory cause we have to call LoadMusic instead of Load
			if (musicDirectory.has_value() && fs::equivalent(it->path(), *musicDirectory)) {
				it.disable_recursion_pending();
			}

			continue;
		}

		if (it->is_regular_file()) {
			Load(it->path());
		}
	}

	if (!musicDirectory.has_value())
		return;

	for (auto& entry : fs::recursive_directory_iterator{ *musicDirectory }) {
		if (entry.is_directory())
			continue;

		if (entry.is_regular_file())
			LoadMusic(entry.path());
	}
}

#define LOAD_AND_EMPLACE(Name, LoadFunc) \
	m_##Name##Vec.emplace_back(LoadFunc(pathStr.c_str())); \
	m_##Name##Map[assetName] = m_##Name##Vec.size() - 1

#define LOAD_INFO_AND_EMPLACE(Type, Name, LoadFunc) \
	Type info; \
	info.pData = LoadFunc(pathStr.c_str(), &info.count); \
	m_##Name##Vec.push_back(std::move(info)); \
	m_##Name##Map[assetName] = m_##Name##Vec.size() - 1

void AssetManager::Load(const fs::path& path) {
	auto extension = path.extension().string();
	std::ranges::transform(extension, extension.begin(), [](auto c) {
		return static_cast<char>(std::tolower(c));
	});

	auto pathStr = path.string();
	auto assetName = path.stem().string();

	// Texture
	if (extension == ".png" || extension == ".bmp" || extension == ".tga" ||
		extension == ".jpg" || extension == ".jpeg" || extension == ".gif" ||
		extension == ".pic" || extension == ".ppm" || extension == ".psd")
	{
		LOAD_AND_EMPLACE(Textures, LoadTexture);
	}
	// Font
	else if (extension == ".ttf" || extension == ".otf" ||
			 extension == ".fnt" || extension == ".bdf")
	{
		LOAD_AND_EMPLACE(Fonts, LoadFont);
	}
	// Model + AnimData
	else if (extension == ".obj" || extension == ".iqm" || extension == ".glb" ||
			 extension == ".gltf" || extension == ".vox" || extension == ".m3d")
	{
		LOAD_AND_EMPLACE(Models, LoadModel);

		if (extension == ".iqm" || extension == ".m3d" ||
			extension == ".gltf" || extension == ".glb")
		{
			LOAD_INFO_AND_EMPLACE(ModelAnimationInfo, ModelAnims, LoadModelAnimations);
		}
	}
	// Materials
	else if (extension == ".mtl")
	{
		LOAD_INFO_AND_EMPLACE(MaterialInfo, Materials, LoadMaterials);
	}
	// Sound
	else if (extension == ".wav" || extension == ".ogg" || extension == ".mp3" ||
			 extension == ".qoa" || extension == ".flac")
	{
		LOAD_AND_EMPLACE(Sounds, LoadSound);
	}
	else
	{
		std::cerr << "Invalid asset path: " << path << std::endl;
	}
}

void AssetManager::LoadMusic(const fs::path& path) {
	auto extension = path.extension().string();
	std::ranges::transform(extension, extension.begin(), [](auto c) {
		return static_cast<char>(std::tolower(c));
	});

	auto pathStr = path.string();
	auto assetName = path.stem().string();
	
	if (extension == ".wav" || extension == ".ogg" || extension == ".mp3" ||
		extension == ".qoa" || extension == ".flac" || extension == ".xm" ||
		extension == ".mod")
	{
		LOAD_AND_EMPLACE(Music, LoadMusicStream);
	}
	else
	{
		std::cerr << "Invalid asset type: " << path << std::endl;
	}
}

#undef LOAD_INFO_AND_EMPLACE
#undef LOAD_AND_EMPLACE

void AssetManager::unload_all() {
#define UNLOAD_COLLECTION(Name, UnloadFunc) \
	for (auto& asset : m_##Name##Vec) { \
		UnloadFunc(asset); \
	} \
	m_##Name##Vec.clear(); \
	m_##Name##Map.clear()
#define UNLOAD_INFO_COLLECTION(Name, UnloadFunc) \
	for (auto& info : m_##Name##Vec) { \
		for (int i = 0; i < info.count; ++i) { \
			UnloadFunc(info.pData[i]); \
		} \
		RL_FREE(info.pData); \
	} \
	m_##Name##Vec.clear(); \
	m_##Name##Map.clear()

	UNLOAD_COLLECTION(Textures, UnloadTexture);
	UNLOAD_COLLECTION(Fonts, UnloadFont);
	UNLOAD_COLLECTION(Models, UnloadModel);
	UNLOAD_COLLECTION(Sounds, UnloadSound);
	UNLOAD_COLLECTION(Music, UnloadMusicStream);
	UNLOAD_INFO_COLLECTION(ModelAnims, UnloadModelAnimation);
	UNLOAD_INFO_COLLECTION(Materials, UnloadMaterial);

#undef UNLOAD_INFO_MAP
#undef UNLOAD_MAP
}
