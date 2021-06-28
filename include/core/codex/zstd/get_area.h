// Copyright (C) 2021 by Mark Melton
//

#pragma once
#include <zstd.h>
#include "core/codex/util/buffer.h"
#include "core/codex/util/get_area.h"

namespace zstd
{

// Encapsulate accessing ZSTD (de)compression output by wrapping the
// ZSTD_outBuffer in a minimal API that allows structured access to
// the data (see core::GetArea for the interface).
//
class GetArea : public core::GetArea {
public:
    // Create an area of the given `capacity` for holding the ZSTD
    // output.
    GetArea(size_t capacity)
	: core::GetArea(capacity) {
	buffer_.dst = begin();
	clear();
    }

    // Prepare to receive output from ZSTD.
    void clear() {
	buffer_.pos = 0;
	buffer_.size = capacity();
    }

    // Update the get area with new output from ZSTD.
    void update() {
	ptr_ = begin();
	end_ = ptr_ + buffer_.pos;
    }

    // Return a pointer to the ZSTD output buffer object.
    ZSTD_outBuffer *buffer() {
	return &buffer_;
    }

private:
    ZSTD_outBuffer buffer_;
};

}; // zstd
