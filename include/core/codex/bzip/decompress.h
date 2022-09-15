// Copyright (C) 2021, 2022 by Mark Melton
//

#pragma once
#include <string>

namespace bzip {

std::string decompress(const char *begin, size_t count);
std::string decompress(const char *begin, const char *end);
std::string decompress(const std::string& str);

}; // bzip
