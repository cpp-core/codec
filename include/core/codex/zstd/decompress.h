// Copyright (C) 2019, 2021, 2022 by Mark Melton
//

#pragma once
#include <istream>

namespace zstd
{

std::string decompress(const char *input_buffer, size_t input_size);
std::string decompress(std::string_view str);

template<class InStream, class OutStream>
void decompress(InStream& source, OutStream& sink);

}; // zstd
