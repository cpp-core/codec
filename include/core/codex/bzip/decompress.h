// Copyright (C) 2021 by Mark Melton
//

#pragma once
#include "core/common.h"

namespace bzip {

string decompress(const char *begin, size_t count);
string decompress(const char *begin, const char *end);
string decompress(const string& str);

}; // bzip
