// Copyright (C) 2021, 2022 by Mark Melton
//

#pragma once
#include <string>
#include "core/codex/util/buffer.h"

namespace core
{

// Provide an interface for getting characters from a buffered area.
//
class GetArea : public core::BufferedArea {
public:
    // Allocate a buffer of `capacity` bytes, set `begin` to the
    // start of the buffer and `count` to the capacity.
    GetArea(unsigned int capacity)
	: BufferedArea(capacity)
	, ptr_(nullptr)
	, end_(nullptr) {
    }

    // Return true if there are character to be read.
    bool available() const { return ptr_ < end_; }

    size_t size() const { return end_ - ptr_; }

    // Return the current character without advancing.
    char peek() const { return *ptr_; }

    // Return a view of the available characters.
    std::string_view view() const { return {ptr_, (size_t)(end_ - ptr_)}; }

    // Return a pointer to the first available character.
    const char *data() const { return ptr_; }

    // Return the current character and advance to the next character.
    char consume() { return *ptr_++; }

    // Advance to the next `n` characters.
    void discard(size_t n) { ptr_ += n; }

protected:
    const char *ptr_, *end_;
};

}; // core
