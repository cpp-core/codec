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

// GetArea, PutArea
//
// Certain compression interfaces (.e.g. bzlib, zstd) expect the
// client to maintain buffers for both sending / receiving data to /
// from the library. The libraries read / write data directly to /
// from a pointer with a capacity that is owned by the library but
// managed by both the library and the client.
//
// The GetArea and PutArea classes provide crisp interfaces to make
// this interplay more explicit. References to the libraries `begin`
// and `count` variables are fed to the constructors and updated to
// reflect the allocated buffer space.
//
// From the libraries perspective, data should be read / written starting at
// `begin` up to `begin` + `count`.
//
// From the client perspective, data should be read / written using the API
// calls.
//

// Provide a buffer from which to 'get' characters.
//
class GetAreaBase {
public:
    // Allocate a buffer of `capacity` bytes, set `begin` to the
    // start of the buffer and `count` to the capacity.
    GetAreaBase(uint capacity)
	: capacity_(capacity)
	, block_(std::make_unique<char[]>(capacity_))
	, ptr_(nullptr)
	, end_(nullptr) {
    }

    char *begin() { return block_.get(); }

    // Return true if there are character to be read.
    bool available() const { return ptr_ < end_; }

    // Return the total capacity of the area.
    uint capacity() const { return capacity_; }

    // Return the current character without advancing.
    char peek() const { return *ptr_; }

    // Return a view of the available characters.
    string_view view() const { return {ptr_, (size_t)(end_ - ptr_)}; }

    // Return the current character and advance to the next character.
    char consume() { return *ptr_++; }

    // Advance to the next `n` characters.
    void discard(size_t n) { ptr_ += n; }

private:
    size_t capacity_;
    std::unique_ptr<char[]> block_;
    
protected:
    const char *ptr_, *end_;
};

class GetArea : public GetAreaBase {
public:
    GetArea(size_t capacity)
	: GetAreaBase(capacity) {
	buffer_.dst = begin();
	clear_pre();
    }

    void clear_pre() {
	buffer_.pos = 0;
	buffer_.size = capacity();
    }

    void update_post() {
	ptr_ = begin();
	end_ = ptr_ + buffer_.pos;
    }
    
    ZSTD_outBuffer *buffer() {
	return &buffer_;
    }
    
    ZSTD_outBuffer buffer_;
};

class PutAreaBase {
public:
    PutAreaBase(size_t capacity)
	: capacity_(capacity)
	, block_(std::make_unique<char[]>(capacity_))
    { }

    char *begin() { return block_.get(); }
    size_t capacity() { return capacity_; }
    
private:
    size_t capacity_;
    std::unique_ptr<char[]> block_;
};

class StreamPutArea : public PutAreaBase {
public:
    StreamPutArea(size_t capacity)
	: PutAreaBase(capacity)
    {
	buffer_.src = begin();
	buffer_.pos = 0;
	buffer_.size = 0;
    }
    
    void update(size_t offset, size_t count) {
	buffer_.pos = offset;
	buffer_.size = count;
    }

    bool empty() const {
	return buffer_.pos >= buffer_.size;
    }

    ZSTD_inBuffer *buffer() {
	return &buffer_;
    }

private:
    ZSTD_inBuffer buffer_;
};

class PutArea {
public:
    PutArea()
    {
	clear();
    }
    
    bool empty() const {
	return buffer_.pos >= buffer_.size;
    }

    void clear() {
	buffer_.src = nullptr;
	buffer_.pos = 0;
	buffer_.size = 0;
    }
    
    void update(const char *begin, const char *end) {
	buffer_.src = begin;
	buffer_.pos = 0;
	buffer_.size = end - begin;
    }

    ZSTD_inBuffer *buffer() {
	return &buffer_;
    }

private:
    ZSTD_inBuffer buffer_;
};

}; // zstd
