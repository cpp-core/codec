// Copyright (C) 2021 by Mark Melton
//

#pragma once
#include <fstream>
#include "core/codex/zstd/decompressor.h"

namespace zstd
{

class FileDecompressor : public Decompressor<std::ifstream> {
public:
    using Base = Decompressor<std::ifstream>;
    FileDecompressor(const string& file)
	: Base(std::ifstream{file}) {
    }

    FileDecompressor(FileDecompressor&& other) 
	: Base(std::move(other)) {
    }
private:
};

}; // zstd
