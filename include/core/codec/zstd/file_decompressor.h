// Copyright (C) 2021, 2022 by Mark Melton
//

#pragma once
#include <fstream>
#include "core/codec/zstd/decompressor.h"

namespace zstd
{

class FileDecompressor : public Decompressor<std::ifstream> {
public:
    using Base = Decompressor<std::ifstream>;
    FileDecompressor(const std::string& file)
	: Base(std::ifstream{file}) {
    }

    FileDecompressor(FileDecompressor&& other) 
	: Base(std::move(other)) {
    }
private:
};

}; // zstd
