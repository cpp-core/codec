// Copyright (C) 2021 by Mark Melton
//

#pragma once
#include "core/codex/bzip/get_area.h"
#include "core/codex/bzip/put_area.h"
#include "core/codex/bzip/new_stream.h"

namespace bzip {

template<class Source>
class Decompressor {
public:
    Decompressor(Source& source, size_t n = 65536);
    ~Decompressor();

    void close();
    bool read_line(string& line);
    bool underflow();

private:
    Source& source_;
    std::unique_ptr<bz_stream> stream_;
    GetArea get_;
    PutArea put_;
};

}; // bzip
