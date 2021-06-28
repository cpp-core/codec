// Copyright (C) 2021 by Mark Melton
//

#pragma once
#include "core/common.h"

namespace core
{

// Provide a simple character buffer of the given capacity.
//
class BufferedArea {
 public:
    // Construct a character buffer of the given `capacity`.
    BufferedArea(size_t capacity)
	: capacity_(capacity)
	, block_(std::make_unique<char[]>(capacity_))
    { }

    // Return a pointer to the start of the buffer.
    char *begin() { return block_.get(); }

    // Return a pointer to just past the end of the buffer.
    char *end() { return begin() + capacity(); }

    // Return the size of buffer.
    size_t capacity() const { return capacity_; }
    
 private:
    size_t capacity_;
    std::unique_ptr<char[]> block_;
};


}; // core
