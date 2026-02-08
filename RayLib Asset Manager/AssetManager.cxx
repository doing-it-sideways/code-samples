#include <cassert>
#include "Types.hpp"

template <std::convertible_to<std::filesystem::path>... AssetPaths>
void AssetManager::load(AssetPaths... nonMusicPaths) {
	(Load(nonMusicPaths), ...);
}

template <std::convertible_to<std::filesystem::path>... AssetPaths>
void AssetManager::load_music(AssetPaths... musicPaths) {
	(LoadMusic(musicPaths), ...);
}

template <RayLibAsset AssetType>
auto& AssetManager::get(std::string_view assetName) {

#define CHECK_ASSET(Type, Map) \
	if constexpr (std::is_same_v<AssetType, Type>) { \
		if (auto asset = Map.find(assetName); asset != Map.end()) { \
			return asset->second; \
		} \
		else { \
			assert(!"Couldn't find the asset!"); \
		} \
	}

	CHECK_ASSET(Texture, m_Textures)
	CHECK_ASSET(Font, m_Fonts)
	CHECK_ASSET(Model, m_Models)
	CHECK_ASSET(ModelAnimationInfo, m_ModelAnims)
	CHECK_ASSET(MaterialInfo, m_Materials)
	CHECK_ASSET(Sound, m_Sounds)
	CHECK_ASSET(Music, m_Music)

#undef CHECK_ASSET
	CAKE_UNREACHABLE;
}
