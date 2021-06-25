// Copyright (C) 2021 by Mark Melton
//

#pragma once
#include <zstd.h>
#include "core/common.h"

namespace zstd
{

template<class T>
struct ZstdBufferAdapter;

template<>
struct ZstdBufferAdapter<ZSTD_inBuffer> {
    static void set_ptr(ZSTD_inBuffer *buffer, void *ptr) {
	buffer->src = ptr;
    }

    static void reset(ZSTD_inBuffer *buffer, size_t capacity) {
	buffer->pos = 0;
	buffer->size = 0;
    }
};

template<>
struct ZstdBufferAdapter<ZSTD_outBuffer> {
    static void set_ptr(ZSTD_outBuffer *buffer, void *ptr) {
	buffer->dst = ptr;
    }
    
    static void reset(ZSTD_outBuffer *buffer, size_t capacity) {
	buffer->pos = 0;
	buffer->size = capacity;
    }
};

// Helper class for managing get and put areas (analogous to a
// streambuf) for the buffers used in ZSTD compression /
// decompression.
template<class T>
class ZstdArea {
public:
    // Construct an area with a buffer of `size` bytes and initialize
    // the corresponding ZSTD buffer. For a put area (ZSTD input
    // buffer), set the cursor and size to zero, i.e. no bytes written
    // yet. For a get area (ZSTD output buffer), set the cursor to
    // zero and size to capacity, i.e. ready to receive bytes.
    ZstdArea(size_t size)
	: block_(std::make_unique<char[]>(size))
	, capacity_(size)
    {
	ZstdBufferAdapter<T>::set_ptr(&buffer_, block_.get());
	reset();
    }

    // Return a pointer to the first character.
    char *ptr_base() const { return block_.get(); }

    // Return a pointer to the current character.
    char *ptr_position() const { return ptr_base() + position(); }

    // Return a pointer to the next available slot for a character.
    char *ptr_size() const { return ptr_base() + size(); }

    // Return a pointer to the end of the allocated area.
    char *ptr_capacity() const { return ptr_base() + capacity(); }

    // Return the position index.
    size_t position() const { return buffer_.pos; }

    // Return the character count.
    size_t size() const { return buffer_.size; }

    // Return `true` iff there are no more characters to be processed in
    // the area; otherwise, `false`.
    bool empty() const { return position() >= size(); }

    // Return the underlying buffer capacity.
    size_t capacity() const { return capacity_; }

    // Insert as many characters from the range `begin` to `end` into
    // the area as possible and return the next character to be
    // inserted or end.
    const char *insert(const char *begin, const char *end) {
	const char *last = std::min(begin + capacity(), end);
	std::copy(begin, last, block_.get());
	buffer_.pos = 0;
	buffer_.size = last - begin;
	return last;
    }

    // Set the position index and count for the area.
    void set_area(size_t p, size_t n) {
	buffer_.pos = p;
	buffer_.size = n;
    }

    // Reset the buffer. For a put area (ZSTD input buffer), set the
    // cursor and size to zero, i.e. no bytes written yet. For a get
    // area (ZSTD output buffer), set the cursor to zero and size to
    // capacity, i.e. ready to receive bytes.
    void reset() {
	ZstdBufferAdapter<T>::reset(zbuffer(), capacity());
    }

    // Return a pointer to the underlying ZSTD buffer.
    T *zbuffer() { return &buffer_; }

    // Return a string representation of the area.
    string repr() const {
	auto str = string_view{ptr_base(), position()};
	return fmt::format("pos {}, size {}: {}", position(), size(), str);
    }
    
private:
    std::unique_ptr<char[]> block_;
    size_t capacity_;
    T buffer_;
};

using ZstdPutArea = ZstdArea<ZSTD_inBuffer>;
using ZstdGetArea = ZstdArea<ZSTD_outBuffer>;

}; // zstd
