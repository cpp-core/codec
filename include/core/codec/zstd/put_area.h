// Copyright (C) 2021, 2022 by Mark Melton
//

#pragma once
#include <zstd.h>
#include "core/codec/util/buffer.h"

namespace zstd
{

// Encapsulate ZSTD decompression input by wrapping the
// ZSTD_inBuffer in a minimal API.
//
class PutArea : public core::BufferedArea {
public:
    // Construct an area of the given `capacity` for holding the ZSTD
    // input.
    PutArea(size_t capacity);

    // Return true if ZSTD has consumed all the input data.
    bool empty() const;

    // Update the put area with data that has been read externally.
    void update(size_t offset, size_t count);

    // Return a pointer to the ZSTD input buffer object.
    ZSTD_inBuffer *buffer() { return &buffer_; }

private:
    ZSTD_inBuffer buffer_;
};

// Encapsulate ZSTD compression input by wrapping the
// ZSTD_inBuffer in a minimal API.
//
class UnbufferedPutArea {
public:
    // Construct a empty put area.
    UnbufferedPutArea();

    // Return true if ZSTD has consumed all the input data.
    bool empty() const;

    // Clear the put area.
    void clear();

    // Update the put area with new client data.
    void update(const char *begin, const char *end);

    // Return a pointer to the ZSTD input buffer object.
    ZSTD_inBuffer *buffer() { return &buffer_; }

private:
    ZSTD_inBuffer buffer_;
};

}; // zstd
