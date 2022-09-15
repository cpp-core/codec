// Copyright (C) 2021, 2022 by Mark Melton
//

#pragma once
#include "core/util/common.h"

namespace bzip {

string decompress(const char *begin, size_t count);
string decompress(const char *begin, const char *end);
string decompress(const std::string& str);

}; // bzip
