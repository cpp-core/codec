// Copyright (C) 2021 by Mark Melton
//

#pragma once
#include <bzlib.h>
#include "core/codex/bzip/get_area.h"

namespace bzip {

template<class Sink>
class Compressor {
public:
    Compressor(Sink& sink, size_t n = 65536);
    ~Compressor();

    void close();
    void write(const char *input, size_t input_len);
    
    template<class T>
    void write_pod(T& value) { write(reinterpret_cast<const char*>(&value), sizeof(T)); }

private:
    Sink& sink_;
    std::unique_ptr<bz_stream> stream_;
    GetArea get_;
};

}; // bzip
