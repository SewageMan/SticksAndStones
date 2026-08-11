#pragma once

#include <concepts>

template<typename T>
concept all_nums = std::integral<T> || std::floating_point<T>;