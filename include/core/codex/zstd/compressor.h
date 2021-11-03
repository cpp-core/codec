// Copyright (C) 2021 by Mark Melton
//

#pragma once
#include <memory>
#include <type_traits>
#include <zstd.h>
#include "core/common.h"
#include "core/codex/zstd/get_area.h"
#include "core/codex/zstd/put_area.h"
#include "core/codex/zstd/exception.h"

namespace zstd
{

// Write bytes to a `Stream` compressed using ZSTD streaming.
//
// The `Stream` object must either:
//
// 1. Have a `write` method with the signautre: write(const char*
// data, size_t count), e.g. std::ostream.
//
// 2. Implement a specialization of `zstd::InStreamadapter` with the
// required `write` method.
//
template<class Sink>
class Compressor {
public:
    // Construct a compressor that will write to the output stream
    // <os> using a buffer of size `n` (defaults to size suggested by
    // ZSTD library).
    explicit Compressor(std::add_rvalue_reference_t<Sink> os, size_t n = 0);

    // Move construct from other.
    Compressor(Compressor&& other);

    // Flush any remaining data to the output stream <os> and cleanup
    // internal allocations.
    ~Compressor();

    // Flush any remaining data to the output stream <os> and cleanup
    // internal allocations.
    void close();

    // Return the number of bytes appended to the output stream.
    size_t count() const { return count_; }

    // Write the data from `begin` up to `end` to `Sink` using ZSTD
    // streaming compression.
    void write(const char *begin, const char *end);

    // Write the data from `begin` to `begin` + `count` to `Sink`
    // using ZSTD streaming compression.
    void write(const char *begin, size_t count) { write(begin, begin + count); }

    // Write the raw bytes representing the pod-type `value` to the `Sink`.
    template<class T>
    void write_pod(T& value) { write(reinterpret_cast<const char*>(&value), sizeof(T)); }

    // Return a reference to the put area for writing data.
    UnbufferedPutArea& put() { return put_; }

    // Return a reference to the put area for writing data.
    const UnbufferedPutArea& put() const { return put_; }

    // Return a reference to the get area for reading data.
    GetArea& get() { return get_; }

    // Return a reference to the get area for reading data.
    const GetArea& get() const { return get_; }

private:
    Sink os_;
    ZSTD_CStream *zsc_;
    UnbufferedPutArea put_;
    GetArea get_;
    size_t count_{0};
};

template<class S> explicit Compressor(S&&) -> Compressor<S>;
template<class S> explicit Compressor(S&&, size_t) -> Compressor<S>;

}; // zstd
