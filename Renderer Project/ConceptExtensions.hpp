#pragma once

#include <concepts>
#include <type_traits>
#include <array>
#include <glm/glm.hpp>

namespace cyber {

template <typename T>
concept arithmetic = std::is_arithmetic_v<T>;

template <typename T, typename... Ts>
concept all_same = (std::is_same_v<T, Ts> && ...);

template <typename T, typename... Ts>
concept all_same_no_extent = (all_same<std::remove_extent_t<T>, std::remove_extent_t<Ts>...>);

template <typename T>
struct is_bounded_array : std::false_type {
	using underlying = T;
};

template <typename T>
struct is_bounded_array<T[]> : std::false_type {
	using underlying = T;
};

template <typename T, std::size_t N>
struct is_bounded_array<T[N]> : std::true_type {
	using underlying = T;
	static constexpr std::size_t len = N;
};

template <typename T, std::size_t N>
struct is_bounded_array<std::array<T, N>> : std::true_type {
	using underlying = T;
	static constexpr std::size_t len = N;
};

template <typename>
struct is_glm_vec : std::false_type {};

template <typename T, glm::length_t L, glm::qualifier Q>
struct is_glm_vec<glm::vec<L, T, Q>> : std::true_type {
	static constexpr glm::length_t len = L;
	static constexpr glm::qualifier qual = Q;
};

template <typename T>
concept glm_vec = is_glm_vec<T>::value;

template <typename>
struct is_glm_mat : std::false_type {};

template <typename T, glm::length_t C, glm::length_t R, glm::qualifier Q>
struct is_glm_mat<glm::mat<C, R, T, Q>> : std::true_type {
	static constexpr glm::length_t cols = C;
	static constexpr glm::length_t rows = R;
	static constexpr glm::qualifier qual = Q;
};

template <typename T>
concept glm_mat = is_glm_mat<T>::value;

template <typename T>
concept std140_element = arithmetic<T> || glm_vec<T> || glm_mat<T>;

template <typename T>
concept std140_elem_arr = is_bounded_array<T>::value &&
						  std140_element<typename is_bounded_array<T>::underlying>;

template <typename... Ts>
concept std140 = ((std140_element<Ts> || std140_elem_arr<Ts>) && ...);

template <typename... Ts>
concept std430 = (std140<Ts...>);

}
