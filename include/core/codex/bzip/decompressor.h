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
    // Construct a decompressor that reads from `source` using a
    // buffer of size `n`.
    Decompressor(Source& source, size_t n = 65536);

    // Destructs a dcompressor freeing any resources.
    ~Decompressor();

    // Free resources and the close the underlying stream.
    void close();

    // Attempt to read the next decompressed line. Return `true` if a
    // (possibly empty) line was read and place the characters in
    // `line`. If there are no more characters to be read, return
    // `false` and set `line` to nil.
    bool read_line(string& line);

    // Attempt to read the next chunk of decompressed characters into
    // the get area (discarding any existing characters) and update
    // the get area pointers discarding any Return `true` if
    // characters are successfully read, `false` otherwise.
    bool underflow();

    // Return a view of the current get area, i.e. the characters that
    // are ready to be read.
    string_view view() const { return get_.view(); }

private:
    Source& source_;
    std::unique_ptr<bz_stream> stream_;
    GetArea get_;
    PutArea put_;
};

}; // bzip
