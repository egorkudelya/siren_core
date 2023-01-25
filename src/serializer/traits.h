#pragma once

#include <iostream>
#include <map>
#include <unordered_map>
#include <vector>

template<typename T>
using is_string_type = std::is_same<std::string, std::decay_t<T>>;

template<typename T>
struct is_map : std::false_type {
};

template<typename Key, typename Value, typename Order, typename Allocator>
struct is_map<std::map<Key, Value, Order, Allocator>> : std::true_type {
};

template<typename Key, typename Value, typename Order, typename Allocator>
struct is_map<std::unordered_map<Key, Value, Order, Allocator>> : std::true_type {
};

template<typename T>
struct is_vector : std::false_type {
};

template<typename T>
struct is_vector<std::vector<T>> : std::true_type {
};
