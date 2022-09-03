// Copyright (C) 2021, 2022 by Mark Melton
//

#pragma once
#include <bzlib.h>
#include "core/utility/common.h"

namespace bzip {

inline bz_stream *new_stream() {
    auto stream = new bz_stream;
    stream->next_in = nullptr;
    stream->avail_in = 0;
    stream->next_out = nullptr;
    stream->avail_out = 0;
    stream->bzalloc = nullptr;
    stream->bzfree = nullptr;
    stream->opaque = nullptr;
    return stream;
}

}; // bzip
