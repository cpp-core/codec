// Copyright (C) 2021 by Mark Melton
//

#include <sstream>
#include "core/codex/bzip/compress.h"
#include "core/codex/bzip/compressor.h"

namespace bzip {

string compress(const char *begin, size_t count) {
    std::stringstream ss;
    Compressor c{ss};
    c.write(begin, count);
    c.close();
    return ss.str();
}

string compress(const char *begin, const char *end) {
    return compress(begin, end - begin);
}

string compress(string_view str) {
    return compress(str.data(), str.size());
}


}; // bzip
