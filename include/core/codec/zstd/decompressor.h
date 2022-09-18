// Copyright (C) 2021, 2022 by Mark Melton
//

#pragma once
#include "core/codec/zstd/get_area.h"
#include "core/codec/zstd/put_area.h"

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
// std::string s, line;
// while (d.read_line(line))
//    s += line;
//
// Low-level interface:
//
// Repeatedly call `underflow` to populate the get area and read the
// data directly from the the get area.
//
// Decompressor d{cin};
// std::string s;
// while (d.underflow())
//     s += std::string_view{d.get().ptr_base(), d.get().ptr_position()};
// 
template<class Source>
class Decompressor {
public:
    // Construct a decompressor that reads from stream `is` using a
    // buffer of size `n` (defaults to the buffer size suggested by
    // the ZSTD library).
    explicit Decompressor(std::add_rvalue_reference_t<Source> is, size_t n = 0);

    // Destruct a decompressor.
    ~Decompressor();

    // Move construct from other.
    Decompressor(Decompressor&& other);

    // Move construct from other adjusting source reference.
    Decompressor(Decompressor&& other, Source& is);

    // Return a reference to the underlying stream.
    Source& stream() { return is_; }
    
    // Free resources and close the underlying stream.
    void close();

    // Attempt to read the next decompressed line. Return `true` if a
    // (possibly empty) line was read and place the characters in
    // `line`. If there are no more characters to be read, return
    // `false` and set `line` to nil.
    bool read_line(std::string& line);

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
    std::string_view view() const { return get_.view(); }

    // Return a reference to the get area for writing data.
    GetArea& get() { return get_; }

    // Return a reference to tht get area for writing data.
    const GetArea& get() const { return get_; }

private:
    // Return a reference to the put area for writing data.
    PutArea& put() { return put_; }

    // Return a reference to tht put area for writing data.
    const PutArea& put() const { return put_; }

    Source is_;
    ZSTD_DStream *zsd_;
    PutArea put_;
    GetArea get_;
};

template<class S> explicit Decompressor(S&&) -> Decompressor<S>;
template<class S> explicit Decompressor(S&&, size_t) -> Decompressor<S>;

}; // zstd
