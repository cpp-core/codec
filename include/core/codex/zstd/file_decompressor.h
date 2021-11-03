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
	, Base(*static_cast<std::ifstream*>(this)) {
    }

    FileDecompressor(FileDecompressor&& other)
	: std::ifstream(std::move(static_cast<std::ifstream&>(other)))
	, Base(std::move(static_cast<Base&>(other))) {
    }
};

}; // zstd
