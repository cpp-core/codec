// Copyright (C) 2021, 2022 by Mark Melton
//

#pragma once
#include <fstream>
#include "core/codec/zstd/compressor.h"

namespace zstd
{

class FileCompressor : public Compressor<std::ofstream> {
public:
    using Base = Compressor<std::ofstream>;
    using Base::write;
    
    FileCompressor(const std::string& file)
	: Base(std::ofstream{file}) {
    }

    FileCompressor(FileCompressor&& other)
	: Base(std::move(other)) {
    }
};

}; // zstd
