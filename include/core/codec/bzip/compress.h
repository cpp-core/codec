// Copyright (C) 2021, 2022 by Mark Melton
//

#pragma once
#include <string>

namespace bzip {

std::string compress(const char *begin, size_t count);
std::string compress(const char *begin, const char *end);
std::string compress(std::string_view str);

}; // bzip
