// Copyright (C) 2021, 2022 by Mark Melton
//

#pragma once
#include <bzlib.h>
#include "core/codec/bzip/get_area.h"

namespace bzip {

// Write bytes compressed using the bzip2 library to `Sink`.
//
template<class Sink>
class Compressor {
public:
    // Construct a compressor that will write to `sink` using a buffer
    // of size `n`.
    Compressor(Sink& sink, size_t n = 65536);

    // Flush any remaining data and free all resources.
    ~Compressor();

    // Flush any remaining data and free all resources.
    void close();

    // Write the data from `begin` to `being` + `count` to the `Sink`.
    void write(const char *begin, size_t count);

    // Write the raw bytes representing the pod-type `value` to the `Sink`.
    template<class T>
    void write_pod(T& value) { write(reinterpret_cast<const char*>(&value), sizeof(T)); }

private:
    Sink& sink_;
    std::unique_ptr<bz_stream> stream_;
    GetArea get_;
};

}; // bzip
