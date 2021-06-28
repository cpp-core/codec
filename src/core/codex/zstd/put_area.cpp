// Copyright (C) 2021 by Mark Melton
//

#include "core/codex/zstd/put_area.h"

namespace zstd
{

PutArea::PutArea(size_t capacity)
    : core::BufferedArea(capacity) {
    buffer_.src = begin();
    buffer_.pos = 0;
    buffer_.size = 0;
}

bool PutArea::empty() const {
    return buffer_.pos >= buffer_.size;
}

void PutArea::update(size_t offset, size_t count) {
    buffer_.pos = offset;
    buffer_.size = count;
}

UnbufferedPutArea::UnbufferedPutArea() {
    clear();
}

bool UnbufferedPutArea::empty() const {
    return buffer_.pos >= buffer_.size;
}

void UnbufferedPutArea::clear() {
    buffer_.src = nullptr;
    buffer_.pos = 0;
    buffer_.size = 0;
}

void UnbufferedPutArea::update(const char *begin, const char *end) {
    buffer_.src = begin;
    buffer_.pos = 0;
    buffer_.size = end - begin;
}

}; // zstd
