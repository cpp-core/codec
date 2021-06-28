// Copyright (C) 2021 by Mark Melton
//

#include <istream>
#include <sstream>
#include "core/codex/bzip/put_area.h"

namespace bzip {

PutArea::PutArea(char *&next, uint& avail, uint capacity)
    : core::BufferedArea(capacity)
    , next_(next)
    , avail_(avail) {
}

void PutArea::update(size_t count) {
    next_ = begin();
    avail_ = count;
}

}; // bzip

