// Copyright (C) 2021 by Mark Melton
//

#pragma once
#include "core/codex/zstd/get_area.h"
#include "core/codex/zstd/put_area.h"

namespace zstd
{

// Read bytes from a `Stream` compressed using ZSTD streaming.
//
// The `Stream` object must either:
//
// 1. Have a `read` method with the signature: read(char* data, size_t
// count), e.g. std::istream.
//
// 2. Implement a specialization of `zstd::InStreamAdapter` with the
// required `read` method.
//
// High-level interface:
//
// Decompressor d{cin};
// string s, line;
// while (d.read_line(line))
//    s += line;
//
// Low-level interface:
//
// Repeatedly call `underflow` to populate the get area and read the
// data directly from the the get area.
//
// Decompressor d{cin};
// string s;
// while (d.underflow())
//     s += string_view{d.get().ptr_base(), d.get().ptr_position()};
// 
template<class Stream>
class Decompressor {
public:
    // Construct a decompressor that reads from stream `is` using a
    // buffer of size `n` (defaults to the buffer size suggested by
    // the ZSTD library).
    Decompressor(Stream& is, size_t n = 0);

    // Destruct a decompressor.
    ~Decompressor();

    // Free resources and close the underlying stream.
    void close();

    // Attempt to read the next decompressed line. Return `true` if a
    // (possibly empty) line was read and place the characters in
    // `line`. If there are no more characters to be read, return
    // `false` and set `line` to nil.
    bool read_line(string& line);

    // Attempt to read up to `count` decompressed bytes placing them
    // into buffer. Return the number of bytes read.
    size_t read_bytes(char *buffer, size_t count);

    // Attempt to read decompressed bytes representing the pod type T
    // into `value`. Return `true` if the value is successfully read;
    // otherwise, return `false`.
    template<class T>
    bool read_pod(T& value) { return read_bytes((char*)&value, sizeof(T)) == sizeof(T); }

    // Attempt to read the next chunk of decompressed characters into
    // the get area (discarding any existing characters) and update
    // the get area pointers discarding any Return `true` if
    // characters are successfully read, `false` otherwise.
    bool underflow();

    // Return a view of the current get area, i.e. the characters that
    // are ready to be read.
    string_view view() const { return get_.view(); }

    // Return a reference to the get area for writing data.
    GetArea& get() { return get_; }

    // Return a reference to tht get area for writing data.
    const GetArea& get() const { return get_; }

private:
    // Return a reference to the put area for writing data.
    PutArea& put() { return put_; }

    // Return a reference to tht put area for writing data.
    const PutArea& put() const { return put_; }

    Stream& is_;
    ZSTD_DStream *zsd_;
    PutArea put_;
    GetArea get_;
};


}; // zstd
