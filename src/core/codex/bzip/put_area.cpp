// Copyright (C) 2021 by Mark Melton
//

#include <istream>
#include <sstream>
#include "core/codex/bzip/put_area.h"

namespace bzip {

PutArea::PutArea(char *&next, uint& avail, uint capacity)
    : next_(next)
    , avail_(avail)
    , capacity_(capacity)
    , buffer_(std::make_unique<char[]>(capacity_))
{
    next_ = buffer_.get();
    avail_ = 0;
}

template<class Source>
bool PutArea::read(Source& source) {
    source.read(buffer_.get(), capacity());
    next_ = buffer_.get();
    avail_ = source.gcount();
    return available();
}

}; // bzip

template bool bzip::PutArea::read(std::istream&);
template bool bzip::PutArea::read(std::stringstream&);

