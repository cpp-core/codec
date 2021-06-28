// Copyright (C) 2021 by Mark Melton
//

#pragma once
#include <zstd.h>
#include "core/codex/util/buffer.h"

namespace zstd
{

class PutArea : public core::BufferedArea {
public:
    PutArea(size_t capacity)
	: core::BufferedArea(capacity)
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

class UnbufferedPutArea {
public:
    UnbufferedPutArea()
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
