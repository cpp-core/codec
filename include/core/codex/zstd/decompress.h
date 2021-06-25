// Copyright (C) 2019, 2021 by Mark Melton
//

#pragma once
#include <istream>
#include "core/common.h"

namespace zstd
{

string decompress(const char *input_buffer, size_t input_size);
string decompress(string_view str);

template<class InStream, class OutStream>
void decompress(InStream& source, OutStream& sink);

}; // zstd
