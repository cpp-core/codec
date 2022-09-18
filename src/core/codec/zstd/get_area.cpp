// Copyright (C) 2021, 2022 by Mark Melton
//

#include "core/codec/zstd/get_area.h"

namespace zstd {

GetArea::GetArea(size_t capacity)
    : core::GetArea(capacity) {
    buffer_.dst = begin();
    clear();
}

void GetArea::clear() {
    buffer_.pos = 0;
    buffer_.size = capacity();
}

void GetArea::update() {
    ptr_ = begin();
    end_ = ptr_ + buffer_.pos;
}

}; // zstd
