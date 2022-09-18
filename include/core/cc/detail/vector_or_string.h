// Copyright (C) 2019, 2022 by Mark Melton
//

#pragma once
#include <string>
#include <vector>

namespace core
{

template<class T>
struct vector_or_string
{ using type = std::vector<T>; };

template<>
struct vector_or_string<char>
{ using type = std::string; };

template<class T>
using vector_or_string_t = typename vector_or_string<T>::type;

}; // core
