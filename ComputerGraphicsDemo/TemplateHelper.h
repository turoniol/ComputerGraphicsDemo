#pragma once

#include <concepts>
#include <utility>

template <class T, class Arr, std::size_t... ints>
T construct_from_array(Arr&& arr, std::index_sequence<ints...>) {
	return T{ arr[ints]...	};
}