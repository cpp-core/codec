// Copyright (C) 2021 by Mark Melton
//

#pragma once
#include <fstream>
#include "core/codex/zstd/decompressor.h"

namespace zstd
{

class FileDecompressor : private std::ifstream, public Decompressor<std::ifstream> {
public:
    using Base = Decompressor<std::ifstream>;
    
    FileDecompressor(const string& file)
	: std::ifstream(file)
	, Base(*(std::ifstream*)this) {
    }
};

}; // zstd
