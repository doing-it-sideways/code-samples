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

#define CHECK_ASSET_INDEX(Type, Name) \
	if constexpr (std::is_same_v<AssetType, Type>) { \
		if (auto asset = m_##Name##Map.find(assetName); asset != m_##Name##Map.end()) { \
			return asset->second; \
		} \
		else { \
			assert(!"Couldn't find the asset!"); \
			return ~0; \
		} \
	}

template <RayLibAsset AssetType>
std::size_t AssetManager::get_index(std::string_view assetName) {
	CHECK_ASSET_INDEX(Texture, Textures)
	CHECK_ASSET_INDEX(Font, Fonts)
	CHECK_ASSET_INDEX(Model, Models)
	CHECK_ASSET_INDEX(ModelAnimationInfo, ModelAnims)
	CHECK_ASSET_INDEX(MaterialInfo, Materials)
	CHECK_ASSET_INDEX(Sound, Sounds)
	CHECK_ASSET_INDEX(Music, Music)

	CAKE_UNREACHABLE;
}

// TODO: replace "return Type{};" with an exception?
#define CHECK_ASSET_FROM_INDEX(Type, Name) \
	if constexpr (std::is_same_v<AssetType, Type>) { \
		if (index < m_##Name##Vec.size()) [[likely]] \
			return m_##Name##Vec[index]; \
		else [[unlikely]] { \
			assert(!"Index out of bounds!"); \
			/*return Type{};*/ \
		} \
	}

template <RayLibAsset AssetType>
auto& AssetManager::get_from_index(std::size_t index) {
	CHECK_ASSET_FROM_INDEX(Texture, Textures)
	CHECK_ASSET_FROM_INDEX(Font, Fonts)
	CHECK_ASSET_FROM_INDEX(Model, Models)
	CHECK_ASSET_FROM_INDEX(ModelAnimationInfo, ModelAnims)
	CHECK_ASSET_FROM_INDEX(MaterialInfo, Materials)
	CHECK_ASSET_FROM_INDEX(Sound, Sounds)
	CHECK_ASSET_FROM_INDEX(Music, Music)

	CAKE_UNREACHABLE;
}

#define CHECK_ASSET(Type) \
	if constexpr (std::is_same_v<AssetType, Type>) { \
		return get_from_index<Type>(get_index<Type>(assetName)); \
	}

template <RayLibAsset AssetType>
auto& AssetManager::get(std::string_view assetName) {
	CHECK_ASSET(Texture)
	CHECK_ASSET(Font)
	CHECK_ASSET(Model)
	CHECK_ASSET(ModelAnimationInfo)
	CHECK_ASSET(MaterialInfo)
	CHECK_ASSET(Sound)
	CHECK_ASSET(Music)

	CAKE_UNREACHABLE;
}

#undef CHECK_ASSET
#undef CHECK_ASSET_INDEX
#undef CHECK_ASSET_FROM_INDEX

