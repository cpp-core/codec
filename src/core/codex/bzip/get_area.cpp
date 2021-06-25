// Copyright (C) 2021 by Mark Melton
//

#include "core/codex/bzip/get_area.h"

namespace bzip {

GetArea::GetArea(char *&next, uint& avail, uint capacity)
    : next_(next)
    , avail_(avail)
    , capacity_(capacity)
    , buffer_(std::make_unique<char[]>(capacity_))
    , ptr_(nullptr)
{
    clear();
}

char GetArea::consume() {
    auto c = *ptr_++;
    return c;
}

void GetArea::clear() {
    next_ = buffer_.get();
    avail_ = capacity();
    ptr_ = next_;
}

}; // bzip
