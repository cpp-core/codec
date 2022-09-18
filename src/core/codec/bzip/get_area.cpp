// Copyright (C) 2021, 2022 by Mark Melton
//

#include "core/codex/bzip/get_area.h"

namespace bzip {

GetArea::GetArea(char *&next, unsigned int& avail, unsigned int capacity)
    : core::GetArea(capacity)
    , next_(next)
    , avail_(avail) {
    next_ = begin();
    avail_ = 0;
}

void GetArea::clear() {
    next_ = begin();
    avail_ = capacity();
}

void GetArea::update() {
    ptr_ = begin();
    end_ = next_;
}

}; // bzip
