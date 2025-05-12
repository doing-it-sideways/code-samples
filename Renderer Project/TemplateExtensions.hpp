/*****************************************************************************
  Some helper templates.
  Since any version prior to C++26 can use variadic template accessors
  (Ts...[x]), have to result to creating recursive templated structs.

  Author(s): Evan O'Bryant
  Copyright © 2024-2025 Evan O'Bryant.    
*****************************************************************************/

#pragma once

#include <cstddef>
#include <type_traits>
#include <glm/glm.hpp>

namespace cyber {

template <std::size_t N, typename T, typename... Ts>
struct nthType {
	using type = typename nthType<N - 1, Ts...>::type;
};

template <typename T, typename... Ts>
struct nthType<0, T, Ts...> {
	using type = T;
};

template <std::size_t N, typename... Ts>
using nthType_t = typename nthType<N, Ts...>::type;

template <typename... Ts>
using firstType_t = typename nthType<0, Ts...>::type;

template <typename... Ts>
using lastType_t = typename nthType<sizeof...(Ts) - 1, Ts...>::type;

} // namespace cyber
