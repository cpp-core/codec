// Copyright (C) 2021, 2022 by Mark Melton
//

#pragma once
#include "core/util/common.h"

namespace bzip {

string compress(const char *begin, size_t count);
string compress(const char *begin, const char *end);
string compress(string_view str);

}; // bzip
