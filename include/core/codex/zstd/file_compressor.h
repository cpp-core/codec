// Copyright (C) 2021 by Mark Melton
//

#pragma once
#include <fstream>
#include "core/codex/zstd/compressor.h"

namespace zstd
{

class FileCompressor : private std::ofstream, public Compressor<std::ofstream> {
public:
    using Base = Compressor<std::ofstream>;
    using Base::write;
    
    FileCompressor(const string& file)
	: std::ofstream(file)
	, Base(*(std::ofstream*)this) {
    }
};

}; // zstd
